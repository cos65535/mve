/*
 * Base classes for all kind of 3D images (or 2D + channels).
 * Written by Simon Fuhrmann.
 */

#ifndef MVE_IMAGE_BASE_HEADER
#define MVE_IMAGE_BASE_HEADER

#include <vector>
#include <stdint.h>

#include "util/string.h"
#include "util/refptr.h"
#include "defines.h"

MVE_NAMESPACE_BEGIN

/**
 * Identifiers for image types.
 */
enum ImageType
{
    IMAGE_TYPE_UNKNOWN,
    /* Unsigned integer types. */
    IMAGE_TYPE_UINT8, // uint8_t, unsigned char
    IMAGE_TYPE_UINT16, // uint16_t
    IMAGE_TYPE_UINT32, // uint32_t, unsigned int
    IMAGE_TYPE_UINT64, // uint64_t
    /* Signed integer types. */
    IMAGE_TYPE_SINT8, // int8_t, char, signed char
    IMAGE_TYPE_SINT16, // int16_t
    IMAGE_TYPE_SINT32, // int32_t, int
    IMAGE_TYPE_SINT64, // int64_t
    /* Floating point types. */
    IMAGE_TYPE_FLOAT, // float
    IMAGE_TYPE_DOUBLE // double
};

/* ---------------------------------------------------------------- */

/**
 * Base class for images without type information.
 * This class basically provides width, height and channel
 * information and a framework for type information and data access.
 */
class ImageBase
{
public:
    typedef util::RefPtr<ImageBase> Ptr;
    typedef util::RefPtr<ImageBase const> ConstPtr;

protected:
    std::size_t w, h, c;

public:
    /** Initializes members with 0. */
    ImageBase (void);
    virtual ~ImageBase (void);

    /** Duplicates the image. Data holders need to reimplement this. */
    virtual ImageBase::Ptr duplicate (void) const;

    /** Returns the width of the image. */
    std::size_t width (void) const;
    /** Returns the height of the image. */
    std::size_t height (void) const;
    /** Returns the amount of channels in the image. */
    std::size_t channels (void) const;

    /**
     * Re-interprets the dimensions of the image. This will fail and
     * return false if the total image size does not match the old image.
     */
    bool reinterpret (std::size_t new_w, std::size_t new_h, std::size_t new_c);

    /** Returns false if one of width, height or channels is 0. */
    bool valid (void) const;

    /** Value type information. Returns UNKNOWN if not overwritten. */
    virtual ImageType get_type (void) const;
    /** Generic byte size information. Returns 0 if not overwritten. */
    virtual std::size_t get_byte_size (void) const;
    /** Pointer to image data. Returns 0 if not overwritten. */
    virtual char const* get_byte_pointer (void) const;
    /** Pointer to image data. Returns 0 if not overwritten. */
    virtual char* get_byte_pointer (void);
    /** Returns a string representation of the image data type. */
    virtual char const* get_type_string (void) const;
};

/* ---------------------------------------------------------------- */

/**
 * Base class for images of arbitrary type. Image values are stored
 * in a standard STL Vector. Type information is provided. This class
 * makes no assumptions about the image structure, i.e. it provides no
 * pixel access methods.
 */
template <typename T>
class TypedImageBase : public ImageBase
{
public:
    typedef T ValueType;
    typedef util::RefPtr<TypedImageBase<T> > Ptr;
    typedef util::RefPtr<TypedImageBase<T> const> ConstPtr;
    typedef std::vector<T> ImageData;

protected:
    ImageData data;

public:
    /** Default ctor creates an empty image. */
    TypedImageBase (void);

    /** Copy ctor duplicates another image. */
    TypedImageBase (TypedImageBase<T> const& other);

    /** Template copy ctor converts from another image. */
    template <typename O>
    TypedImageBase (TypedImageBase<O> const& other);

    virtual ~TypedImageBase (void);

    /** Duplicates the image. Data holders need to reimplement this. */
    virtual ImageBase::Ptr duplicate (void) const;

    /** Allocates new image space, clearing previous content. */
    void allocate (std::size_t width, std::size_t height, std::size_t chans);

    /**
     * Resizes the underlying image data vector.
     * Note that this leaves the remaining image data unchanged.
     * Warning: If the image is shrunk, the data vector is resized but
     * still consumes the original amount of memory. Use allocate()
     * instead if the previous data is not important.
     */
    void resize (std::size_t width, std::size_t height, std::size_t chans);

    /** Clears the image data from memory. */
    virtual void clear (void);

    /** Fills the data with a constant value. */
    void fill (T const& value);

    /** TODO Fills the data with constant value per channel. */
    // Better place in mve::Image because knows about image structure?
    //void fill (T const* values);

    /** Swaps the contents of the images. */
    void swap (TypedImageBase<T>& other);

    /** Value type information by template specialization. */
    virtual ImageType get_type (void) const;
    /** Returns a string representation of the image data type. */
    char const* get_type_string (void) const;

    /** Returns the data vector for the image. */
    ImageData const& get_data (void) const;
    /** Returns the data vector for the image. */
    ImageData& get_data (void);

    /** Returns the data pointer. */
    T const* get_data_pointer (void) const;
    /** Returns the data pointer. */
    T* get_data_pointer (void);

    /** Returns data pointer to beginning. */
    T* begin (void);
    /** Returns const data pointer to beginning. */
    T const* begin (void) const;
    /** Returns data pointer to end. */
    T* end (void);
    /** Returns const data pointer to end. */
    T const* end (void) const;

    /** Returns the amount of pixels in the image (w * h). */
    std::size_t get_pixel_amount (void) const;
    /** Returns the amount of values in the image (w * h * c). */
    std::size_t get_value_amount (void) const;

    /** Returns the size of the image in bytes (w * h * c * BPV). */
    std::size_t get_byte_size (void) const;
    /** Returns the char pointer to the data. */
    char const* get_byte_pointer (void) const;
    /** Returns the char pointer to the data. */
    char* get_byte_pointer (void);

    /* ------------------------- Operators ------------------------ */

    // Assignment already works with default operator=
    //TypedImageBase<T>& operator= (TypedImageBase<T> const& other);
};

/* ================================================================ */

inline
ImageBase::ImageBase (void)
    : w(0), h(0), c(0)
{
}

inline
ImageBase::~ImageBase (void)
{
}

inline ImageBase::Ptr
ImageBase::duplicate (void) const
{
    return ImageBase::Ptr(new ImageBase(*this));
}

inline std::size_t
ImageBase::width (void) const
{
    return this->w;
}

inline std::size_t
ImageBase::height (void) const
{
    return this->h;
}

inline std::size_t
ImageBase::channels (void) const
{
    return this->c;
}

inline bool
ImageBase::reinterpret (std::size_t new_w, std::size_t new_h, std::size_t new_c)
{
    if (new_w * new_h * new_c != this->w * this->h * this->c)
        return false;

    this->w = new_w;
    this->h = new_h;
    this->c = new_c;
    return true;
}

inline bool
ImageBase::valid (void) const
{
    return this->w && this->h && this->c;
}

inline ImageType
ImageBase::get_type (void) const
{
    return IMAGE_TYPE_UNKNOWN;
}

inline std::size_t
ImageBase::get_byte_size (void) const
{
    return 0;
}

inline char const*
ImageBase::get_byte_pointer (void) const
{
    return 0;
}

inline char*
ImageBase::get_byte_pointer (void)
{
    return 0;
}

inline char const*
ImageBase::get_type_string (void) const
{
    return "unknown";
}

/* ---------------------------------------------------------------- */

template <typename T>
inline
TypedImageBase<T>::TypedImageBase (void)
{
}

template <typename T>
inline
TypedImageBase<T>::TypedImageBase (TypedImageBase<T> const& other)
    : ImageBase(other), data(other.data)
{
}

template <typename T>
template <typename O>
inline
TypedImageBase<T>::TypedImageBase (TypedImageBase<O> const& other)
{
    this->allocate(other.width(), other.height(), other.channels());
    typename TypedImageBase<O>::ImageData const& odata(other.get_data());
    //std::transform(odata.begin(), odata.end(), data.begin(), static_cast<T>);
    for (std::size_t i = 0; i < odata.size(); ++i)
        this->data[i] = static_cast<T>(odata[i]);
}

template <typename T>
inline
TypedImageBase<T>::~TypedImageBase (void)
{
}

template <typename T>
inline ImageBase::Ptr
TypedImageBase<T>::duplicate (void) const
{
    return ImageBase::Ptr(new TypedImageBase<T>(*this));
}

template <typename T>
inline ImageType
TypedImageBase<T>::get_type (void) const
{
    return IMAGE_TYPE_UNKNOWN;
}

template <>
inline ImageType
TypedImageBase<int8_t>::get_type (void) const
{
    return IMAGE_TYPE_SINT8;
}

template <>
inline ImageType
TypedImageBase<int16_t>::get_type (void) const
{
    return IMAGE_TYPE_SINT16;
}

template <>
inline ImageType
TypedImageBase<int32_t>::get_type (void) const
{
    return IMAGE_TYPE_SINT32;
}

template <>
inline ImageType
TypedImageBase<int64_t>::get_type (void) const
{
    return IMAGE_TYPE_SINT64;
}

template <>
inline ImageType
TypedImageBase<uint8_t>::get_type (void) const
{
    return IMAGE_TYPE_UINT8;
}

template <>
inline ImageType
TypedImageBase<uint16_t>::get_type (void) const
{
    return IMAGE_TYPE_UINT16;
}

template <>
inline ImageType
TypedImageBase<uint32_t>::get_type (void) const
{
    return IMAGE_TYPE_UINT32;
}

template <>
inline ImageType
TypedImageBase<uint64_t>::get_type (void) const
{
    return IMAGE_TYPE_UINT64;
}

template <>
inline ImageType
TypedImageBase<float>::get_type (void) const
{
    return IMAGE_TYPE_FLOAT;
}

template <>
inline ImageType
TypedImageBase<double>::get_type (void) const
{
    return IMAGE_TYPE_DOUBLE;
}

template <typename T>
inline char const*
TypedImageBase<T>::get_type_string (void) const
{
    return util::string::for_type<T>();
}

template <typename T>
inline void
TypedImageBase<T>::allocate (std::size_t width,
    std::size_t height, std::size_t chans)
{
    this->clear();
    this->resize(width, height, chans);
}

template <typename T>
inline void
TypedImageBase<T>::resize (std::size_t width,
    std::size_t height, std::size_t chans)
{
    this->w = width;
    this->h = height;
    this->c = chans;
    this->data.resize(width * height * chans);
}

template <typename T>
inline void
TypedImageBase<T>::clear (void)
{
    this->w = 0;
    this->h = 0;
    this->c = 0;
    this->data.clear();
}

template <typename T>
inline void
TypedImageBase<T>::fill (T const& value)
{
    std::fill(this->data.begin(), this->data.end(), value);
}

template <typename T>
inline void
TypedImageBase<T>::swap (TypedImageBase<T>& other)
{
    std::swap(this->w, other.w);
    std::swap(this->h, other.h);
    std::swap(this->c, other.c);
    std::swap(this->data, other.data);
}

template <typename T>
inline typename TypedImageBase<T>::ImageData&
TypedImageBase<T>::get_data (void)
{
    return this->data;
}

template <typename T>
inline typename TypedImageBase<T>::ImageData const&
TypedImageBase<T>::get_data (void) const
{
    return this->data;
}

template <typename T>
inline T const*
TypedImageBase<T>::get_data_pointer (void) const
{
    return &this->data[0];
}

template <typename T>
inline T*
TypedImageBase<T>::get_data_pointer (void)
{
    return &this->data[0];
}

template <typename T>
inline T*
TypedImageBase<T>::begin (void)
{
    return &this->data[0];
}

template <typename T>
inline T const*
TypedImageBase<T>::begin (void) const
{
    return &this->data[0];
}

template <typename T>
inline T*
TypedImageBase<T>::end (void)
{
    return &this->data[0] + this->data.size();
}

template <typename T>
inline T const*
TypedImageBase<T>::end (void) const
{
    return &this->data[0] + this->data.size();
}

template <typename T>
inline std::size_t
TypedImageBase<T>::get_pixel_amount (void) const
{
    return this->w * this->h;
}

template <typename T>
inline std::size_t
TypedImageBase<T>::get_value_amount (void) const
{
    return this->data.size();
}

template <typename T>
inline std::size_t
TypedImageBase<T>::get_byte_size (void) const
{
    return this->data.size() * sizeof(T);
}

template <typename T>
inline char const*
TypedImageBase<T>::get_byte_pointer (void) const
{
    return (char const*)&this->data[0];
}

template <typename T>
inline char*
TypedImageBase<T>::get_byte_pointer (void)
{
    return (char*)&this->data[0];
}

MVE_NAMESPACE_END

/* ---------------------------------------------------------------- */

STD_NAMESPACE_BEGIN

/** Specialization of std::swap for efficient image swapping. */
template <class T>
inline void
swap (mve::TypedImageBase<T>& a, mve::TypedImageBase<T>& b)
{
    a.swap(b);
}

STD_NAMESPACE_END

#endif /* MVE_IMAGE_BASE_HEADER */
