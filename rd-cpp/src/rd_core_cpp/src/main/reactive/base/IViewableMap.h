#ifndef RD_CPP_IVIEWABLEMAP_H
#define RD_CPP_IVIEWABLEMAP_H

#include "LifetimeDefinition.h"
#include "overloaded.h"
#include "interfaces.h"
#include "viewable_collections.h"
#include "core_util.h"

#include "thirdparty.hpp"

#include <unordered_map>

namespace rd {
	/**
	 * \brief A set allowing its contents to be observed.
	 * \tparam K type of stored keys (may be abstract) 
	 * \tparam V type of stored values (may be abstract)
	 */
	template<typename K, typename V>
	class IViewableMap
			: public IViewable<std::pair<K const *, V const *>> {
	protected:
		using WK = value_or_wrapper<K>;
		using WV = value_or_wrapper<V>;
		using OV = opt_or_wrapper<V>;

		mutable std::unordered_map<
				Lifetime,
				ordered_map<K const *, LifetimeDefinition, wrapper::TransparentHash < K>, wrapper::TransparentKeyEqual<K>>
		>
		lifetimes;
	public:
		/**
		 * \brief Represents an addition, update or removal of an element in the map.
		 */
		class Event {
		public:
			class Add {
			public:
				K const *key;
				V const *new_value;

				Add(K const *key, V const *new_value) : key(key), new_value(new_value) {}
			};

			class Update {
			public:
				K const *key;
				V const *old_value;
				V const *new_value;

				Update(K const *key, V const *old_value, V const *new_value) : key(key), old_value(old_value),
																			   new_value(new_value) {}
			};

			class Remove {
			public:
				K const *key;
				V const *old_value;

				Remove(K const *key, V const *old_value) : key(key), old_value(old_value) {}
			};

			variant<Add, Update, Remove> v;

			Event(Add const &x) : v(x) {}

			Event(Update const &x) : v(x) {}

			Event(Remove const &x) : v(x) {}

			K const *get_key() const {
				return visit(util::make_visitor(
						[](typename Event::Add const &e) {
							return e.key;
						},
						[](typename Event::Update const &e) {
							return e.key;
						},
						[](typename Event::Remove const &e) {
							return e.key;
						}
				), v);
			}

			V const *get_old_value() const {
				return visit(util::make_visitor(
						[](typename Event::Add const &e) {
							return static_cast<V const *>(nullptr);
						},
						[](typename Event::Update const &e) {
							return e.old_value;
						},
						[](typename Event::Remove const &e) {
							e.old_value;
						}
				), v);
			}

			V const *get_new_value() const {
				return visit(util::make_visitor(
						[](typename Event::Add const &e) {
							return e.new_value;
						},
						[](typename Event::Update const &e) {
							return e.new_value;
						},
						[](typename Event::Remove const &e) {
							return static_cast<V const *>(nullptr);
						}
				), v);
			}

			friend std::string to_string(Event const &e) {
				std::string res = visit(util::make_visitor(
						[](typename Event::Add const &e) -> std::string {
							return "Add " +
								   to_string(*e.key) + ":" +
								   to_string(*e.new_value);
						},
						[](typename Event::Update const &e) -> std::string {
							return "Update " +
								   to_string(*e.key) + ":" +
								   //                       to_string(e.old_value) + ":" +
								   to_string(*e.new_value);
						},
						[](typename Event::Remove const &e) -> std::string {
							return "Remove " +
								   to_string(*e.key);
						}
				), e.v);
				return res;
			}
		};

		//region ctor/dtor

		IViewableMap() = default;

		IViewableMap(IViewableMap &&) = default;

		IViewableMap &operator=(IViewableMap &&) = default;

		virtual ~IViewableMap() = default;
		//endregion

		void view(Lifetime lifetime,
				  std::function< void(Lifetime lifetime,
				  std::pair<K const *, V const *> const &)> handler) const override {
			advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, K const &key, V const &value) {
				const std::pair<K const *, V const *> entry = std::make_pair(&key, &value);
				switch (kind) {
					case AddRemove::ADD: {
						if (lifetimes[lifetime].count(key) == 0) {
							/*auto const &[it, inserted] = lifetimes[lifetime].emplace(key, LifetimeDefinition(lifetime));*/
							auto const &pair = lifetimes[lifetime].emplace(&key, LifetimeDefinition(lifetime));
							auto &it = pair.first;
							auto &inserted = pair.second;
							RD_ASSERT_MSG(inserted,
										  "lifetime definition already exists in viewable map by key:" +
										  to_string(key));
							handler(it->second.lifetime, entry);
						}
						break;
					}
					case AddRemove::REMOVE: {
						RD_ASSERT_MSG(lifetimes.at(lifetime).count(key) > 0,
									  "attempting to remove non-existing lifetime in viewable map by key:" +
									  to_string(key));
						LifetimeDefinition def = std::move(lifetimes.at(lifetime).at(key));
						lifetimes.at(lifetime).erase(key);
						def.terminate();
						break;
					}
				}
			});
		}

		/**
		 * \brief Adds a subscription to additions and removals of map elements. When a map element is updated, the [handler]
		 * is called twice: to report the removal of the old element and the addition of the new one.
		 * \param lifetime lifetime of subscription.
		 * \param handler to be called.
		 */
		void advise_add_remove(Lifetime lifetime, std::function<void(AddRemove, K const &, V const &)> handler) const {
			advise(lifetime, [handler](Event e) {
				visit(util::make_visitor(
						[handler](typename Event::Add const &e) {
							handler(AddRemove::ADD, *e.key, *e.new_value);
						},
						[handler](typename Event::Update const &e) {
							handler(AddRemove::REMOVE, *e.key, *e.old_value);
							handler(AddRemove::ADD, *e.key, *e.new_value);
						},
						[handler](typename Event::Remove const &e) {
							handler(AddRemove::REMOVE, *e.key, *e.old_value);
						}
				), e.v);
			});
		}

		/**
		 * \brief Adds a subscription to changes of the contents of the map, with the handler receiving keys and values 
		 * as separate parameters.
		 * 
		 * \details When [handler] is initially added, it is called receiving all keys and values currently in the map.
		 * Every time a key/value pair is added to the map, the [handler] is called receiving the new key and value.
		 * The [Lifetime] instance passed to the handler expires when the key/value pair is removed from the map.
		 * 
		 * \param lifetime lifetime of subscription.
		 * \param handler to be called.
		 */
		void view(Lifetime lifetime, std::function< void(Lifetime, K const &, V const &)> handler) const {
			view(lifetime, [handler](Lifetime lf, const std::pair<K const *, V const *> entry) {
				handler(lf, *entry.first, *entry.second);
			});
		}

		virtual void advise(Lifetime lifetime, std::function<void(Event)> handler) const = 0;

		virtual const V *get(K const &) const = 0;

		virtual const V *set(WK, WV) const = 0;

		virtual OV remove(K const &) const = 0;

		virtual void clear() const = 0;

		virtual size_t size() const = 0;

		virtual bool empty() const = 0;

		template<typename ... Args>
		const V *emplace_set(WK wk, Args &&... args) const {
			return set(std::move(wk), WV{std::forward<Args>(args)...});
		}
	};
}

static_assert(std::is_move_constructible<rd::IViewableMap<int, int>::Event>::value,
			  "Is move constructible from IViewableMap<int, int>::Event");

#endif //RD_CPP_IVIEWABLEMAP_H