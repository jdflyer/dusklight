#include "cxxopts.hpp"
#include "dusk/assets/assets_pack.hpp"
#include "dusk/assets/assets_unpack.hpp"

namespace dusk::assets {

int assets_main(int argc, char** argv) {
    cxxopts::ParseResult parsed_arg_options;

    try {
        cxxopts::Options arg_options(
            "Dusklight Assets", "Asset Extract/Package Subsystem of Dusklight");

        arg_options.add_options()("assets", "Assets", cxxopts::value<std::string>())("command",
            "Mode to run the converter in (unpack,repack)", cxxopts::value<std::string>())(
            "input", "The Input File/Directory", cxxopts::value<std::filesystem::path>())(
            "output", "The Output File/Directory", cxxopts::value<std::filesystem::path>())(
            "h,help", "Usage", cxxopts::value<std::string>());
        arg_options.parse_positional({"assets", "command", "input", "output"});

        parsed_arg_options = arg_options.parse(argc, argv);

        if (parsed_arg_options.count("help")) {
            printf("%s", (arg_options.help() + "\n").c_str());
            exit(0);
        }
    } catch (const cxxopts::exceptions::exception& e) {
        fprintf(stderr, "Argument Error: %s\n", e.what());
        exit(1);
    }

    const auto command = parsed_arg_options["command"].as<std::string>();
    const auto input = parsed_arg_options["input"].as<std::filesystem::path>();
    const auto output = parsed_arg_options["output"].as<std::filesystem::path>();

    if (command == "unpack") {
        return assets_unpack_main(input, output);
    } else if (command == "pack") {
        return assets_pack_main(input, output);
    }

    return 0;
}

}  // namespace dusk::assets
