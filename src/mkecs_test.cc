// Copyright 2013, Takuya Akiba
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Takuya Akiba nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
