package com.jetbrains.rd.framework

abstract class AbstractBuffer {
    abstract var position: Int

    abstract fun writeByte(value: Byte)
    abstract fun readByte(): Byte

    abstract fun writeShort(value: Short)
    abstract fun readShort(): Short

    abstract fun writeInt(value: Int)
    abstract fun readInt(): Int

    abstract fun writeLong(value: Long)
    abstract fun readLong(): Long

    abstract fun writeFloat(value: Float)
    abstract fun readFloat(): Float

    abstract fun writeDouble(value: Double)
    abstract fun readDouble(): Double

    abstract fun writeBoolean(value: Boolean)
    abstract fun readBoolean(): Boolean

    abstract fun writeChar(value: Char)
    abstract fun readChar(): Char

    abstract fun readByteArray(): ByteArray
    abstract fun readByteArrayRaw(array: ByteArray)

    abstract fun writeByteArray(array: ByteArray)
    abstract fun writeByteArrayRaw(array: ByteArray, count: Int? = null)

    fun readString() = readNullableString()!!
    abstract fun readNullableString(): String?

    fun writeString(value: String) = writeNullableString(value)
    abstract fun writeNullableString(value: String?)

    abstract fun readCharArray(): CharArray
    abstract fun writeCharArray(array: CharArray)

    abstract fun readShortArray(): ShortArray
    abstract fun writeShortArray(array: ShortArray)

    abstract fun readIntArray(): IntArray
    abstract fun writeIntArray(array: IntArray)

    abstract fun readLongArray(): LongArray
    abstract fun writeLongArray(array: LongArray)

    abstract fun readFloatArray(): FloatArray
    abstract fun writeFloatArray(array: FloatArray)

    abstract fun readDoubleArray(): DoubleArray
    abstract fun writeDoubleArray(array: DoubleArray)

    abstract fun readBooleanArray(): BooleanArray
    abstract fun writeBooleanArray(array: BooleanArray)

    abstract fun getArray(): ByteArray

    abstract fun checkAvailable(moreSize: Int)

    @ExperimentalUnsignedTypes
    fun readUByte() = readByte().toUByte()

    @ExperimentalUnsignedTypes
    fun writeUByte(value: UByte) = writeByte(value.toByte())

    @ExperimentalUnsignedTypes
    fun readUShort() = readShort().toUShort()

    @ExperimentalUnsignedTypes
    fun writeUShort(value: UShort) = writeShort(value.toShort())

    @ExperimentalUnsignedTypes
    fun readUInt(): UInt = readInt().toUInt()

    @ExperimentalUnsignedTypes
    fun writeUInt(value: UInt) = writeInt(value.toInt())

    @ExperimentalUnsignedTypes
    fun readULong(): ULong = readLong().toULong()

    @ExperimentalUnsignedTypes
    fun writeULong(value: ULong) = writeLong(value.toLong())

    @ExperimentalUnsignedTypes
    fun readUShortArray() : UShortArray = readShortArray().asUShortArray()

    @ExperimentalUnsignedTypes
    fun writeUShortArray(array: UShortArray) = writeShortArray(array.asShortArray())

    @ExperimentalUnsignedTypes
    fun readUIntArray() : UIntArray = readIntArray().asUIntArray()

    @ExperimentalUnsignedTypes
    fun writeUIntArray(array: UIntArray) = writeIntArray(array.asIntArray())

    @ExperimentalUnsignedTypes
    fun readULongArray() : ULongArray = readLongArray().asULongArray()

    @ExperimentalUnsignedTypes
    fun writeULongArray(array: ULongArray) = writeLongArray(array.asLongArray())
}

expect fun createAbstractBuffer(): AbstractBuffer

fun AbstractBuffer.rewind() {
    this.position = 0
}