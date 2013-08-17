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
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <utility>

using namespace std;

int main(int argc, char **argv) {
  // Arguments
  int K, num_iterations = mkecs::kDefaultNumIterations;
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "usage: mkecs K [num_iterations(=%d)] < graph\n",
            mkecs::kDefaultNumIterations);
    exit(EXIT_FAILURE);
  }
  K = atoi(argv[1]);
  if (argc == 3) num_iterations = atoi(argv[2]);

  // Input
  vector<pair<int, int> > es;
  for (int v, w; 2 == scanf("%d%d", &v, &w); ) {
    es.push_back(make_pair(v, w));
  }

  // Decompose
  vector<vector<int> > mkecs;
  mkecs::Decompose(es, mkecs, K, num_iterations);

  // Output
  for (size_t i = 0; i < mkecs.size(); ++i) {
    const vector<int> &vs = mkecs[i];
    for (size_t j = 0; j < vs.size(); ++j) {
      printf("%d%c", vs[j], j + 1 == vs.size() ? '\n' : ' ');
    }
  }
  exit(EXIT_SUCCESS);
}
