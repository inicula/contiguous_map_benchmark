#include "benchmark.h"
#include "ContiguousMap.h"
#include <random>

auto generate_pairs(size_t N)
{
  std::vector<std::pair<int, int>> res;
  res.reserve(N);
  for(size_t i = 0; i < N; ++i)
  {
    res.push_back(std::make_pair(std::rand(), std::rand()));
  }
  return res;
}

const auto global_pairs = generate_pairs(11000);

static void BM_std_search(benchmark::State& state)
{
  const long count = state.range(0);
  std::map std_map(global_pairs.begin(), global_pairs.begin() + count);
  const auto iterations = 3 * count;
  for(auto _ : state)
  {
    for(size_t i = 0; i < iterations; ++i)
    {
      auto it = std_map.find(std::rand());
      benchmark::DoNotOptimize(it);
    }
  }
}

static void BM_contig_serach(benchmark::State& state)
{
  const long count = state.range(0);
  ContiguousMap<int, int> my_map(global_pairs.cbegin(),
                                 global_pairs.cbegin() + count);
  const auto iterations = 3 * count;
  for(auto _ : state)
  {
    for(size_t i = 0; i < iterations; ++i)
    {
      auto it = my_map.find(std::rand());
      benchmark::DoNotOptimize(it);
    }
  }
}

static void BM_contig_insert(benchmark::State& state)
{
  const long count = state.range(0);
  const long bytes = state.range(0) * 2 * long(sizeof(int));
  for(auto _ : state)
  {
    ContiguousMap<int, int> my_map;
    my_map.insert(global_pairs.cbegin(), global_pairs.cbegin() + count);
    benchmark::DoNotOptimize(my_map);
  }
  state.SetBytesProcessed(long(state.iterations()) * bytes);
  state.SetLabel(std::to_string(bytes / 1024) + "kB");
}

static void BM_std_insert(benchmark::State& state)
{
  const long count = state.range(0);
  const long bytes = state.range(0) * 2 * long(sizeof(int));
  for(auto _ : state)
  {
    std::map<int, int> std_map;
    std_map.insert(global_pairs.cbegin(), global_pairs.cbegin() + count);
    benchmark::DoNotOptimize(std_map);
  }
  state.SetBytesProcessed(long(state.iterations()) * bytes);
  state.SetLabel(std::to_string(bytes / 1024) + "kB");
}

BENCHMARK(BM_contig_insert)
    ->Unit(benchmark::kMillisecond)
    ->RangeMultiplier(2)
    ->Range(3000, 10000);

BENCHMARK(BM_std_insert)
    ->Unit(benchmark::kMillisecond)
    ->RangeMultiplier(2)
    ->Range(3000, 10000);

BENCHMARK(BM_contig_serach)
    ->Unit(benchmark::kMillisecond)
    ->RangeMultiplier(2)
    ->Range(3000, 10000);

BENCHMARK(BM_std_search)
    ->Unit(benchmark::kMillisecond)
    ->RangeMultiplier(2)
    ->Range(3000, 10000);

BENCHMARK_MAIN();
