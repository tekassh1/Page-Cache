#ifndef PAGE_CACHE
#define PAGE_CACHE

#include <chrono>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <unordered_map>

#define CACHE_SIZE 256 * 1024 * 1024  // Bytes
#define PAGE_SIZE 4096

typedef struct CacheBlock {
    int fd;
    int req_am;
    int page_idx;
    std::shared_ptr<char[]> cached_page;
    std::chrono::time_point<std::chrono::steady_clock> access_time;
    CacheBlock(int fd = 0, int req_am = 0, int page_idx = 0, std::shared_ptr<char[]> cached_page = nullptr)
        : fd(fd),
          req_am(req_am),
          page_idx(page_idx),
          cached_page(cached_page),
          access_time(std::chrono::steady_clock::now()) {};
} CacheBlock;

struct PairHasher {
   public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& x) const;
};

typedef std::unordered_map<std::pair<int, int>, CacheBlock, PairHasher> cache_map;  // key = <fd, page num>

class PageCache {
    size_t max_cache_pages_am;

    cache_map cache_blocks;

    struct LFUComparator {
        bool operator()(const CacheBlock& cb1, const CacheBlock& cb2) const {
            if (cb1.req_am == cb2.req_am) {
                return cb1.access_time < cb2.access_time;
            }
            return cb1.req_am < cb2.req_am;
        }
    };

    std::set<CacheBlock, LFUComparator> priority_queue;

    void syncBlock(int fd, int page_idx);
    void displaceBlock();
    void rmPage(int fd, int page_idx);
    void access(int fd, int page_idx);

   public:
    PageCache(size_t cache_size_bytes);
    void syncBlocks(int fd);
    void cachePage(int fd, int page_idx);
    bool pageExist(int fd, int page_idx);
    std::optional<CacheBlock> getCached(int fd, int page_idx);

    void clearFileCaches(int fd);
};

#endif