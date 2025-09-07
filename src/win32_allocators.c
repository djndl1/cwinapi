#include "cwinapi/winallocator.h"
#include "cwinapi/olecom/allocator.h"
#include "cwinapi/winhandle.h"
#include "cwinapi/winerror.h"

#include <objbase.h>

static HANDLE get_allocator_heap(const mem_allocator *const self)
{
    return self->user_data == nullptr
        ? GetProcessHeap() : (HANDLE)self->user_data;
}

winhandle winheap_allocator_handle(const mem_allocator *const self)
{
    HANDLE handle = get_allocator_heap(self);
    if (handle == NULL) {
        return invalid_winhandle;
    }
    return (winhandle) { .handle = handle };
}

static mem_alloc_result allocate_from_heap(const mem_allocator *const self,
                                           size_t byte_count)
{
    HANDLE heapHandle = get_allocator_heap(self);
    if (heapHandle == nullptr) {
        return (mem_alloc_result){ .error = last_error().code };
    }

    void *mem = HeapAlloc(heapHandle, 0, byte_count);
    mem_alloc_result result = { 0 };
    result.mem = mem;
    if (result.mem == nullptr) {
        result.error = last_error().code;
    }
    return result;
}

static void free_from_heap(const mem_allocator * const self,
                           void *mem)
{
    HANDLE heapHandle = get_allocator_heap(self);
    HeapFree(heapHandle, 0, mem);
}

static mem_alloc_result reallocate_from_heap(const mem_allocator *const self,
                                             void *mem,
                                             size_t newsize)
{
    HANDLE heapHandle = get_allocator_heap(self);
    if (heapHandle == nullptr) {
        return (mem_alloc_result){ .error = last_error().code };
    }

    void *newmem = HeapReAlloc(heapHandle, 0, mem, newsize);
    mem_alloc_result result = { 0 };
    result.mem = newmem;
    if (result.mem == nullptr) {
        result.error = last_error().code;
    }
    return result;
}



static const mem_allocator _win32_heap_allocator_table = {
    .user_data = nullptr,
    .allocate = allocate_from_heap,
    .deallocate = free_from_heap,
    .reallocate = reallocate_from_heap,
};

winheap_creation_result winheap_allocator_create(winheap_creation_options options)
{
    DWORD flags = 0;
    if (options.lockless) {
        flags |= HEAP_NO_SERIALIZE;
    }
    if (options.code_executable_from_heap) {
        flags |= HEAP_CREATE_ENABLE_EXECUTE;
    }
    if (options.use_seh_exception) {
        flags |= HEAP_GENERATE_EXCEPTIONS;
    }
    winhandle h = WINHANDLE(HeapCreate(flags,
                                       options.initial_size,
                                       options.maximum_size));
    if (h.handle == nullptr) {
        return (winheap_creation_result){ .status = WIN_LASTERR };
    }

    mem_allocator allocator = (mem_allocator){
        .user_data = h.handle,
        .allocate = allocate_from_heap,
        .deallocate = free_from_heap,
        .reallocate = reallocate_from_heap,
    };
    return (winheap_creation_result){
         .status = WIN_OK,
         .allocator = allocator,
    };
}


winstatus winheap_allocator_destroy(mem_allocator *self)
{
    if (self == nullptr || self->user_data) return WIN_ERR(WINERROR(EINVAL));

    winstatus status = HeapDestroy(self->user_data) ? WIN_OK : WIN_LASTERR;
    if (status.succeeded) {
        self->user_data = nullptr;
        self->allocate = nullptr;
        self->deallocate = nullptr;
        self->reallocate = nullptr;
    }
    return status;
}

winheap_size_result winheap_allocator_largest_free_block_size(const mem_allocator *const self)
{
    if (self == nullptr) {
        return (winheap_size_result){ .status = WIN_ERR(WINERROR(EINVAL)) };
    }

    HANDLE handle = get_allocator_heap(self);
    size_t block_size = HeapCompact(handle, 0);

    if (block_size == 0) {
        return (winheap_size_result){ .status = WIN_LASTERR };
    }

    return (winheap_size_result) { .status = WIN_OK, .size = block_size };
}

static HEAP_SUMMARY winheap_allocator_heap_summary(const mem_allocator* const self)
{
    HEAP_SUMMARY summary = { .cb = sizeof(HEAP_SUMMARY) };
    HANDLE handle = get_allocator_heap(self);
    if (!HeapSummary(handle, 0, &summary)) {
        summary.cb = 0;
    }
    return summary;
}

winheap_size_result winheap_allocator_committed_size(const mem_allocator* const self)
{
    if (self == nullptr) {
        return (winheap_size_result){ .status = WIN_ERR(WINERROR(EINVAL)) };
    }

    HEAP_SUMMARY summary = winheap_allocator_heap_summary(self);
    if (summary.cb == 0) {
        return (winheap_size_result){ .status = WIN_LASTERR };
    }
    return (winheap_size_result){ .size = summary.cbCommitted, .status = WIN_OK };
}

winheap_size_result winheap_allocator_allocated_size(const mem_allocator* const self)
{
    if (self == nullptr) {
        return (winheap_size_result){ .status = WIN_ERR(WINERROR(EINVAL)) };
    }

    HEAP_SUMMARY summary = winheap_allocator_heap_summary(self);
    if (summary.cb == 0) {
        return (winheap_size_result){ .status = WIN_LASTERR };
    }
    return (winheap_size_result){ .size = summary.cbAllocated, .status = WIN_OK };
}

winheap_size_result winheap_allocator_reserved_size(const mem_allocator* const self)
{
    if (self == nullptr) {
        return (winheap_size_result){ .status = WIN_ERR(WINERROR(EINVAL)) };
    }

    HEAP_SUMMARY summary = winheap_allocator_heap_summary(self);
    if (summary.cb == 0) {
        return (winheap_size_result){ .status = WIN_LASTERR };
    }
    return (winheap_size_result){ .size = summary.cbReserved, .status = WIN_OK };
}

winheap_size_result winheap_allocator_max_reserved_size(const mem_allocator* const self)
{
    if (self == nullptr) {
        return (winheap_size_result){ .status = WIN_ERR(WINERROR(EINVAL)) };
    }

    HEAP_SUMMARY summary = winheap_allocator_heap_summary(self);
    if (summary.cb == 0) {
        return (winheap_size_result){ .status = WIN_LASTERR };
    }
    return (winheap_size_result){ .size = summary.cbMaxReserve, .status = WIN_OK };
}

winheap_size_result winheap_allocator_query_block_size(const mem_allocator* const self, const void* mem)
{
    if (self == nullptr) {
        return (winheap_size_result){ .status = WIN_ERR(WINERROR(EINVAL)) };
    }

    HANDLE handle = get_allocator_heap(self);
    winheap_size_result result = { 0 };
    result.size = HeapSize(handle, 0, mem);
    result.status = (result.size == (size_t)-1) ? WIN_LASTERR : WIN_OK;
    return result;
}

const mem_allocator *const process_heap_allocator = &_win32_heap_allocator_table;

static mem_alloc_result com_allocate(const mem_allocator *const self, size_t count)
{
    void *mem = CoTaskMemAlloc(count);
    mem_alloc_result result = { 0 };
    result.mem = mem;
    if (result.mem == nullptr) {
        result.error = 1;
    }
    return result;
}

static mem_alloc_result com_reallocate(const mem_allocator *const self,
                                       void *mem,
                                       size_t count)
{
    void *newmem = CoTaskMemRealloc(mem, count);
    mem_alloc_result result = { 0 };
    result.mem = newmem;
    if (newmem == nullptr) {
        result.error = 1;
    }
    return result;
}

static void com_deallocate(const mem_allocator *const self, void *mem)
{
    CoTaskMemFree(mem);
}

static const mem_allocator _com_allocator_table = {
    .user_data = nullptr,
    .allocate = com_allocate,
    .deallocate = com_deallocate,
    .reallocate = com_reallocate,
};

const mem_allocator *const com_heap_allocator = &_com_allocator_table;


static mem_alloc_result global_allocate(const mem_allocator *const self, size_t count)
{
    mem_alloc_result result = { 0 };
    result.mem = GlobalAlloc(GMEM_FIXED, count);
    if (result.mem == nullptr) {
        result.error = last_error().code;
    }
    return result;
}

static mem_alloc_result global_reallocate(const mem_allocator *const self,
                                          void *mem,
                                          size_t count)
{
    mem_alloc_result result = { 0 };
    result.mem = GlobalReAlloc(mem, count, 0);
    if (result.mem == nullptr) {
        result.error = last_error().code;
    }
    return result;
}

static void global_deallocate(const mem_allocator *const self, void *mem)
{
    GlobalFree(mem);
}

static const mem_allocator _global_allocator_table = {
    .user_data = nullptr,
    .allocate = global_allocate,
    .deallocate = global_deallocate,
    .reallocate = global_reallocate,
};

const mem_allocator *const global_allocator = &_global_allocator_table;

static mem_alloc_result local_allocate(const mem_allocator *const self, size_t count)
{
    mem_alloc_result result = { 0 };
    result.mem = LocalAlloc(LMEM_FIXED, count);
    if (result.mem == nullptr) {
        result.error = last_error().code;
    }
    return result;
}

static mem_alloc_result local_reallocate(const mem_allocator *const self,
                                         void *mem,
                                         size_t count)
{
    mem_alloc_result result = { 0 };
    result.mem = LocalReAlloc(mem, count, 0);
    if (result.mem == nullptr) {
        result.error = last_error().code;
    }
    return result;
}

static void local_deallocate(const mem_allocator *const self, void *mem)
{
    LocalFree(mem);
}

static const mem_allocator _local_allocator_table = {
    .user_data = nullptr,
    .allocate = local_allocate,
    .deallocate = local_deallocate,
    .reallocate = local_reallocate,
};

const mem_allocator *const local_allocator = &_local_allocator_table;
