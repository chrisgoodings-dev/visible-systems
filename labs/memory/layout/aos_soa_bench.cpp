#include <iostream>
#include <vector>
#include <cstdint>
#include <filesystem>
#ifdef _WIN32
#include "metrics_win.hpp"
#endif


#include "bench.hpp"

namespace {

struct ParticleAoS {
    float x, y, z;
    std::uint8_t active; // keep it small, but alignment may add padding
};

struct ParticlesSoA {
    std::vector<float> x, y, z;
    std::vector<std::uint8_t> active;

    explicit ParticlesSoA(std::size_t n)
        : x(n), y(n), z(n), active(n) {}
};

volatile float fsink = 0.0f;

} // namespace

int main(int argc, char** argv) {
    std::size_t N = 5'000'000;
    std::size_t iters = 20;
    if (argc > 1) N = std::stoull(argv[1]);
    if (argc > 2) iters = std::stoull(argv[2]);
	
	std::string out_dir;
	for (int i = 1; i < argc; ++i) {
		std::string a = argv[i];
		if (a == "--out" && i + 1 < argc) out_dir = argv[++i];
	}
	if (!out_dir.empty()) std::filesystem::create_directories(out_dir);

    std::vector<ParticleAoS> aos(N);
    ParticlesSoA soa(N);
	
	vs::RunMetadata base{};
	base.suite = "memory.layout";
	base.build_type =
	#if defined(NDEBUG)
		"Release";
	#else
		"DebugOrRelWithDebInfo";
	#endif

	base.compiler =
	#if defined(_MSC_VER)
		"MSVC " + std::to_string(_MSC_VER);
	#else
		"UnknownCompiler";
	#endif
	base.timestamp_utc = vs::utc_timestamp();
	base.git_commit = vs::get_env("GITHUB_SHA");
	#ifdef _WIN32
	base.sys = vs::get_system_info();
	#endif


    // init
    for (std::size_t i = 0; i < N; ++i) {
        const float f = (float)(i % 1024) * 0.001f;
        aos[i] = ParticleAoS{f, f + 1.0f, f + 2.0f, (std::uint8_t)(i & 1)};
        soa.x[i] = f; soa.y[i] = f + 1.0f; soa.z[i] = f + 2.0f; soa.active[i] = (std::uint8_t)(i & 1);
    }

    std::cout << "aos_soa_bench N=" << N << " iters=" << iters << "\n";
    vs::print_csv_header(std::cout);

    // Iterate positions only: SoA often wins due to tighter contiguous arrays
    {
        #ifdef _WIN32
		{
			vs::WinCounters c{};
			auto r = vs::bench_with_counters("AoS_iterate_positions_only", 5, iters, [&] {
				float acc = 0.0f;
				for (std::size_t i = 0; i < N; ++i) acc += aos[i].x + aos[i].y + aos[i].z;
				fsink += acc;
			}, c);
			vs::print_csv_row(std::cout, r);

			if (!out_dir.empty()) {
				auto meta = base;
				meta.benchmark = r.name;
				meta.counters = c;
				vs::write_json_result((std::filesystem::path(out_dir) / "AoS_iterate_positions_only.json").string(), meta, r);
			}
		}
		#else
			auto r = vs::bench("AoS_iterate_positions_only", 5, iters, [&] {
				float acc = 0.0f;
				for (std::size_t i = 0; i < N; ++i) {
					acc += aos[i].x + aos[i].y + aos[i].z;
				}
				fsink += acc;
			});
			vs::print_csv_row(std::cout, r);
		#endif
    }

    {
		#ifdef _WIN32
		{
			vs::WinCounters c{};
			auto r = vs::bench_with_counters("AoS_iterate_positions_only", 5, iters, [&] {
				float acc = 0.0f;
				for (std::size_t i = 0; i < N; ++i) acc += aos[i].x + aos[i].y + aos[i].z;
				fsink += acc;
			}, c);
			vs::print_csv_row(std::cout, r);

			if (!out_dir.empty()) {
				auto meta = base;
				meta.benchmark = r.name;
				meta.counters = c;
				vs::write_json_result((std::filesystem::path(out_dir) / "AoS_iterate_positions_only.json").string(), meta, r);
			}
		}
		#else
        auto r = vs::bench("SoA_iterate_positions_only", 5, iters, [&] {
            float acc = 0.0f;
            for (std::size_t i = 0; i < N; ++i) {
                acc += soa.x[i] + soa.y[i] + soa.z[i];
            }
            fsink += acc;
        });
        vs::print_csv_row(std::cout, r);
		#endif
    }

    // Iterate positions + active flag (more mixed access)
    {
		#ifdef _WIN32
		{
			vs::WinCounters c{};
			auto r = vs::bench_with_counters("AoS_iterate_positions_only", 5, iters, [&] {
				float acc = 0.0f;
				for (std::size_t i = 0; i < N; ++i) acc += aos[i].x + aos[i].y + aos[i].z;
				fsink += acc;
			}, c);
			vs::print_csv_row(std::cout, r);

			if (!out_dir.empty()) {
				auto meta = base;
				meta.benchmark = r.name;
				meta.counters = c;
				vs::write_json_result((std::filesystem::path(out_dir) / "AoS_iterate_positions_only.json").string(), meta, r);
			}
		}
		#else
        auto r = vs::bench("AoS_iterate_with_active_branch", 5, iters, [&] {
            float acc = 0.0f;
            for (std::size_t i = 0; i < N; ++i) {
                if (aos[i].active) acc += aos[i].x;
            }
            fsink += acc;
        });
        vs::print_csv_row(std::cout, r);
		#endif
    }

    {
		#ifdef _WIN32
		{
			vs::WinCounters c{};
			auto r = vs::bench_with_counters("AoS_iterate_positions_only", 5, iters, [&] {
				float acc = 0.0f;
				for (std::size_t i = 0; i < N; ++i) acc += aos[i].x + aos[i].y + aos[i].z;
				fsink += acc;
			}, c);
			vs::print_csv_row(std::cout, r);

			if (!out_dir.empty()) {
				auto meta = base;
				meta.benchmark = r.name;
				meta.counters = c;
				vs::write_json_result((std::filesystem::path(out_dir) / "AoS_iterate_positions_only.json").string(), meta, r);
			}
		}
		#else
        auto r = vs::bench("SoA_iterate_with_active_branch", 5, iters, [&] {
            float acc = 0.0f;
            for (std::size_t i = 0; i < N; ++i) {
                if (soa.active[i]) acc += soa.x[i];
            }
            fsink += acc;
        });
        vs::print_csv_row(std::cout, r);
		#endif
    }

    std::cerr << "fsink=" << fsink << "\n";
	
    return 0;
}
