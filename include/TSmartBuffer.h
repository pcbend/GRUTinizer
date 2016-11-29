#ifndef _TSMARTBUFFER_H_
#define _TSMARTBUFFER_H_

#ifndef __CINT__
#ifndef __ROOTMACRO__
#   include <mutex>
#   include <atomic>
#   include <memory>
#endif
#endif
//class  std::mutex;

#include "TObject.h"

/// Holds a char buffer and manages its memory.
/**
  TSmartBuffer holds a char buffer, and deletes it at the appropriate time.
  This is done in a threadsafe manner.
  Subsets of the buffer can be made without copying the underlying C-style array.
  The underlying C-style array will be free'd only when no references to the buffer,
    nor to subsets of the buffer, exist.

  TSmartBuffer is intended to be lightweight,
    and cheap to pass by value.
 */
class TSmartBuffer : public TObject {
public:
  /// Constructs an empty TSmartBuffer.
  /**
    The data pointer will be set to NULL
    The size will be equal to zero.
   */
  TSmartBuffer();

  /// Constructs a TSmartBuffer from a C-style array.
  /**
    The buffer must have been allocated using malloc.
    The buffer must not be managed by another TSmartBuffer.
    Once constructed, the buffer belongs to the object,
      which will call free() on it accordingly.
   */
  TSmartBuffer(char* buffer, size_t size);

  /// Destructs the TSmartBuffer, free-ing the array, if necessary.
  /**
    The TSmartBuffer will call free() on the underlying C-style array
      if it is the last reference to that array.
   */
  ~TSmartBuffer();


#ifndef __CINT__
#ifndef __ROOTMACRO__
  /// Copy constructor
  TSmartBuffer(const TSmartBuffer& other) = default;

  /// Assignment operator
  TSmartBuffer& operator=(const TSmartBuffer& other) = default;

  /// Move constructor
  TSmartBuffer(TSmartBuffer&& other) = default;

  TSmartBuffer& operator=(TSmartBuffer&& other) = default;
#endif
#endif

  /// Clears the object.
  /**
    The object will be left in the same state
      as if it had just been constructed by TSmartBuffer().
    This does not need to be called prior to the object begin destructed.
   */
  void Clear();

  /// Sets the current buffer.
  /**
    The object will be left in the same state
      as if it had just been constructed by TSmartBuffer(buffer, size).
   */
  void SetBuffer(char* buffer, size_t size);

  /// Returns the data pointer.
  /**
    If the object does not currently hold a buffer, returns NULL.
    Otherwise, the result is the location of the data buffer.
    Reads from the location of GetData() to GetData()+GetSize()-1 are valid.
   */
  const char* GetData() const { return fData; }

  /// Returns the size of the array
  /**
    The size returned is the size of the current buffer.
    This may not correspond to the size of the buffer passed in,
      depending on the use of BufferSubset.
   */
  size_t GetSize()      const { return fSize; }

  /// Returns a subset of the current array.
  /**
    @param pos The position of the start of the subset of the buffer.
               Should range from 0 to GetSize()-1.

    @param length The length of the subset of the buffer.
                  If equal to -1, then the remainder of the buffer is used.

    The object returned will point to a subset of the the existing buffer.
    The underlying array is not copied.
    free() will only be called on the underlying array
      once no references to the array or any of its subsets remain.
   */
  TSmartBuffer BufferSubset(size_t pos, size_t length = -1) const;

  /// Advances the pointer by the amount given.
  /**
     @param dist The distance by which the pointer should be advanced.

     If the buffer cannot be advanced that far, the pointer
       will be advanced to the end of the buffer.
   */
  void Advance(size_t dist);

  /// Prints a short description of the TSmartBuffer.
  void Print(Option_t* opt = "") const;


private:
  /// Swaps the contents of two TSmartBuffers.
  /**
    For internal use only.
   */
  void swap(TSmartBuffer& other);

  /// The pointer to the current data
  char* fData;

  /// The size of the current subset of the data.
  size_t fSize;

#ifndef __CINT__
#ifndef __ROOTMACRO__
  std::shared_ptr<char> fAllocatedData;
#endif
#endif

  ClassDef(TSmartBuffer,0);
};

#endif /* _TSMARTBUFFER_H_ */
