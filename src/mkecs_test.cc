#include "mkecs.h"
#include <algorithm>
#include <cstdlib>
#include <gtest/gtest.h>

using namespace std;
typedef pair<int, int> pii;

void PrintEdgeList(const vector<pii> &es) {
  for (size_t i = 0; i < es.size(); ++i) {
    fprintf(stderr, "%d %d\n", es[i].first, es[i].second);
  }
}

void PrintMKECS(const vector<vector<int> > &mkecs) {
  for (size_t i = 0; i < mkecs.size(); ++i) {
    const vector<int> &vs = mkecs[i];
    for (size_t j = 0; j < vs.size(); ++j) {
      fprintf(stderr, "%d%c", vs[j], j + 1 == vs.size() ? '\n' : ' ');
    }
  }
}

// {0, 1, ..., n - 1}, {n, n + 1, ..., 2n - 1}, ... (* m times)
void TestSameSizeClusters(const vector<pii> &es, int k, int n, int m) {
  vector<vector<int> > mkecs;
  fprintf(stderr, "<k = %d>\n", k);
  mkecs::Decompose(es, mkecs, k);
  // PrintEdgeList(es);
  // PrintMKECS(k, mkecs);

  ASSERT_EQ(m, mkecs.size());
  sort(mkecs.begin(), mkecs.end());
  for (size_t i = 0; i < mkecs.size(); ++i) {
    vector<int> &s = mkecs[i];
    ASSERT_EQ(n, s.size());
    sort(s.begin(), s.end());
    for (size_t j = 0; j < s.size(); ++j) ASSERT_EQ(n * i + j, s[j]);
  }
}

// Each vertex is incident to |d| edges in the cluster
void GenerateSameSizeClusters(int n, int m, int d, vector<pii> &es) {
  ASSERT_EQ(0, d % 2);  // Due to limit of the generation algorithm

  for (int i = 0; i < m; ++i) {
    vector<int> vs(n);
    for (int j = 0; j < n; ++j) vs[j] = i * n + j;
    for (int a = 0; a < d / 2; ++a) {
      random_shuffle(vs.begin(), vs.end());
      for (int j = 0; j < n; ++j) {
        es.push_back(make_pair(vs[j], vs[(j + 1) % n]));
      }
    }
  }
}

TEST(MKECS, Empty) {
  vector<pii> es;
  TestSameSizeClusters(es,   1, 0, 0);
  TestSameSizeClusters(es,   2, 0, 0);
  TestSameSizeClusters(es,   3, 0, 0);
  TestSameSizeClusters(es,  10, 0, 0);
  TestSameSizeClusters(es, 100, 0, 0);
}

TEST(MKECS, DisconnectedSmall) {
  vector<pii> es;
  GenerateSameSizeClusters(5, 3, 4, es);
  TestSameSizeClusters(es,   1,  5, 3);
  TestSameSizeClusters(es,   2,  5, 3);
  TestSameSizeClusters(es,   3,  5, 3);
  TestSameSizeClusters(es,   4,  5, 3);
  TestSameSizeClusters(es,   5,  0, 0);
  TestSameSizeClusters(es,   6,  0, 0);
}

TEST(MKECS, DisconnectedLarge) {
  vector<pii> es;
  GenerateSameSizeClusters(100, 10, 30, es);
  TestSameSizeClusters(es,   1, 100, 10);
  TestSameSizeClusters(es,  10, 100, 10);
  TestSameSizeClusters(es,  25, 100, 10);
  TestSameSizeClusters(es,  30, 100, 10);
  TestSameSizeClusters(es,  31,   0,  0);
  TestSameSizeClusters(es, 100,   0,  0);
}

TEST(MKECS, Connected) {
  vector<pii> es;
  GenerateSameSizeClusters(100, 10, 30, es);
  for (int i = 0; i < 10; ++i) {
    for (int j = i + 1; j < 10; ++j) {
      es.push_back(make_pair(i * 100 + rand() % 100,
                             j * 100 + rand() % 100));
    }
  }

  TestSameSizeClusters(es,   1, 1000,  1);
  TestSameSizeClusters(es,  30,  100, 10);
  TestSameSizeClusters(es,  31,    0,  0);
}

TEST(MKECS, RandomNoise) {
  vector<pii> es;
  GenerateSameSizeClusters(100, 10, 30, es);
  for (int i = 0; i < 29; ++i) {
    es.push_back(make_pair(rand() % 1000, rand() % 1000));
  }
  for (int v = 1000; v < 1100; ++v) {
    for (int i = 0; i < 29; ++i) {
      es.push_back(make_pair(v, rand() % 1000));
    }
  }

  TestSameSizeClusters(es,  30, 100, 10);
  TestSameSizeClusters(es,  31,   0,  0);
}
