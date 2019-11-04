using System;
using System.Collections.Generic;
using System.Threading;
using JetBrains.Annotations;
using JetBrains.Rd.Impl;
using JetBrains.Serialization;

namespace JetBrains.Rd
{
  public abstract class RdContextBase : IEquatable<RdContextBase>
  {
    [NotNull] public readonly string Key;
    public readonly bool IsHeavy;

    public RdContextBase(string key, bool isHeavy)
    {
      Key = key;
      IsHeavy = isHeavy;
    }

    public static RdContextBase Read(SerializationCtx ctx, UnsafeReader reader)
    {
      return ctx.Serializers.Read<RdContextBase>(ctx, reader);
    }

    public static void Write(SerializationCtx ctx, UnsafeWriter writer, RdContextBase value)
    {
      ctx.Serializers.Write<RdContextBase>(ctx, writer, value);
    }

    public bool Equals(RdContextBase other)
    {
      if (ReferenceEquals(null, other)) return false;
      if (ReferenceEquals(this, other)) return true;
      return Key == other.Key;
    }

    public override bool Equals(object obj)
    {
      if (ReferenceEquals(null, obj)) return false;
      if (ReferenceEquals(this, obj)) return true;
      if (obj.GetType() != this.GetType()) return false;
      return Equals((RdContextBase) obj);
    }

    public override int GetHashCode()
    {
      return Key.GetHashCode();
    }

    public static bool operator ==(RdContextBase left, RdContextBase right)
    {
      return Equals(left, right);
    }

    public static bool operator !=(RdContextBase left, RdContextBase right)
    {
      return !Equals(left, right);
    }

    public abstract void RegisterOn(ProtocolContexts contexts);
    public abstract void RegisterOn(ISerializers serializers);
  }

  /// <summary>
  /// Describes a context and provides access to value associated with this context.
  /// The associated value is thread-local and synchronized between send/advise pairs on <see cref="IWire"/>. The associated value will be the same in handler method in <see cref="IWire.Advise"/> as it was in <see cref="IWire.Send"/>.
  /// Instances of this class with the same <see cref="RdContext.Key"/> will share the associated value.
  /// Best practice is to declare contexts in toplevel entities in protocol model using <c>Toplevel.context</c>. Manual declaration is also possible.
  /// </summary>
  /// <typeparam name="T">The type of value stored by this context</typeparam>
  public abstract class RdContext<T> : RdContextBase
  {
    [NotNull] public readonly CtxReadDelegate<T> ReadDelegate;
    [NotNull] public readonly CtxWriteDelegate<T> WriteDelegate;
    
    private ThreadLocal<Stack<T>> myContextStack = new ThreadLocal<Stack<T>>(() => new Stack<T>());
#if NET35
    private ThreadLocal<T> myValue = new ThreadLocal<T>();
#else
    private AsyncLocal<T> myValue = new AsyncLocal<T>();
#endif

    /// <summary>
    /// 
    /// </summary>
    /// <param name="key">Textual name of this context. This is used to match this with protocol counterparts</param>
    /// <param name="isHeavy">Whether or not this context is heavy. A heavy context maintains a value set and interns values. A light context sends values as-is and does not maintain a value set.</param>
    /// <param name="readDelegate">Serializer to be used with this context.</param>
    /// <param name="writeDelegate">Serializer to be used with this context.</param>
    protected RdContext(string key, bool isHeavy, [NotNull] CtxReadDelegate<T> readDelegate,
      [NotNull] CtxWriteDelegate<T> writeDelegate) : base(key, isHeavy)
    {
      ReadDelegate = readDelegate;
      WriteDelegate = writeDelegate;
    }

    /// <summary>
    /// Current (thread- or async-local) value for this context
    /// </summary>
    public T Value
    {
      get => myValue.Value;
      set => myValue.Value = value;
    }

    /// <summary>
    /// Pushes current context value to a thread-local stack and sets new value
    /// </summary>
    public void PushContext(T value)
    {
      myContextStack.Value.Push(Value);
      Value = value;
    }

    /// <summary>
    /// Restores previous context value from a thread-local stack
    /// </summary>
    public void PopContext()
    {
      Value = myContextStack.Value.Pop();
    }

    public override void RegisterOn(ProtocolContexts contexts)
    {
      contexts.RegisterContext(this);
    }
  }
}