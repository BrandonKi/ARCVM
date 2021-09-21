#include <cstdint>
#include <string>
#include <vector>

namespace arcvm {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

template <typename T>
concept Stringable = requires(T t) {
    std::string(t);
};

struct File {
    std::string file_name;
    std::string file_data;

    template <Stringable T>
    File(T name) : file_name(std::move(name)) {}
};

enum class OptimizationLevel : i8 {
    custom = -1,
    zero = 0,
    one = 1,
    two = 2,
    three = 3
};

enum class Mode : i8 { binary = 0, text = 1 };

struct Args {
    OptimizationLevel opt_level;
    Mode mode;
    std::vector<File> input_files;
    std::string output_file_name;
};

} // namespace arcvm