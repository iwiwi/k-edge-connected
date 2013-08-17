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
#include <cassert>
#include <cstdio>
#include <tr1/unordered_map>
#include <queue>

using namespace std;
using namespace std::tr1;

typedef unordered_map<int, int> umap_ii;
typedef pair<int, int> pii;

namespace mkecs {
class Internal {
 public:
  void Decompose(const vector<pii> &es, vector<vector<int> > &mkecs,
                 int k, int num_iterations);

 private:
  int K, V;

  vector<umap_ii> adj;
  vector<int> deg;

  queue<pii> fc_queue;
  vector<vector<int> > tmp_vs;
  vector<vector<pii> > tmp_es;

  vector<vector<int> > comps_vs;
  vector<vector<pii> > comps_es;

  vector<vector<int> > kcore_adj;
  vector<int> kcore_deg;

  vector<int> uf_par;

  inline int UFRoot(int x) {
    return uf_par[x] < 0 ? x : uf_par[x] = UFRoot(uf_par[x]);
  }

  void AddEdge(int v, int w, int c);
  int Contract(int v, int w);
  void Cut(int v);

  void ComputeKCore(vector<int> &vs, vector<pii> &es);
  void DecomposeIteration(vector<int> vs, vector<pii> es);
};

void Internal::ComputeKCore(vector<int> &vs, vector<pii> &es) {
  // Initialize
  vector<int> &deg = kcore_deg;
  vector<vector<int> > &adj = kcore_adj;
  for (size_t i = 0; i < vs.size(); ++i) {
    deg[vs[i]] = 0;
    adj[vs[i]].clear();
  }
  for (size_t i = 0; i < es.size(); ++i) {
    int v = es[i].first, w = es[i].second;
    adj[v].push_back(w);
    adj[w].push_back(v);
    ++deg[v];
    ++deg[w];
  }

  // BFS-like k-core computation
  queue<int> que;
  for (size_t i = 0; i < vs.size(); ++i) {
    if (deg[vs[i]] < K) que.push(vs[i]);
  }
  while (!que.empty()) {
    int v = que.front();
    que.pop();
    for (size_t i = 0; i < adj[v].size(); ++i) {
      int w = adj[v][i];
      if (deg[w] >= K && --deg[w] < K) que.push(w);
    }
  }

  // Finalize
  for (size_t i = 0; i < vs.size(); ++i) {
    adj[vs[i]].clear();
  }
  vector<int> tvs;
  vector<pii> tes;
  for (size_t i = 0; i < vs.size(); ++i) {
    if (deg[vs[i]] >= K) tvs.push_back(vs[i]);
  }
  for (size_t i = 0; i < es.size(); ++i) {
    int v = es[i].first, w = es[i].second;
    if (deg[v] >= K && deg[w] >= K) tes.push_back(make_pair(v, w));
  }
  vs.swap(tvs);
  es.swap(tes);
}

void Internal::AddEdge(int v, int w, int c) {
  if (v == w) return;

  int d1 = (adj[v][w] += c), d2 = (adj[w][v] += c);
  assert(d1 == d2);
  deg[v] += c;
  deg[w] += c;
  if (d1 >= K) fc_queue.push(make_pair(v, w));
}

int Internal::Contract(int v, int w) {
  if (adj[v].size() < adj[w].size()) swap(v, w);
  uf_par[w] = v;

  for (umap_ii::iterator ite = adj[w].begin(); ite != adj[w].end(); ++ite) {
    int x = ite->first, c = ite->second;
    adj[x].erase(w);
    deg[x] -= c;
    if (x != v) AddEdge(v, x, c);
  }

  adj[w].clear();
  deg[w] = 0;
  return v;
}

void Internal::Cut(int v) {
  for (umap_ii::iterator ite = adj[v].begin(); ite != adj[v].end(); ++ite) {
    int w = ite->first, c = ite->second;
    adj[w].erase(v);
    deg[w] -= c;
  }
  adj[v].clear();
}

void Internal::DecomposeIteration(vector<int> vs, vector<pii> es) {
  if (vs.size() <= 1 || (int)vs.size() < K) return;

  // Initialize
  ComputeKCore(vs, es);
  for (size_t i = 0; i < vs.size(); ++i) {
    int v = vs[i];
    adj[v].clear();
    uf_par[v] = -1;
  }
  for (size_t i = 0; i < es.size(); ++i) {
    AddEdge(es[i].first, es[i].second, 1);
  }

  // Contract and cut
  random_shuffle(es.begin(), es.end());
  for (size_t i = 0; i < es.size(); ++i) {
    // Forced contraction
    while (!fc_queue.empty()) {
      int v = fc_queue.front().first;
      int w = fc_queue.front().second;
      fc_queue.pop();
      v = UFRoot(v);
      w = UFRoot(w);
      if (v == w) continue;
      v = Contract(v, w);
      if (deg[v] < K) Cut(v);
    }

    // Random contraction
    int v = es[i].first, w = es[i].second;
    v = UFRoot(v);
    w = UFRoot(w);
    if (adj[v].count(w) == 0) continue;  // Already removed

    if (deg[v] < K) Cut(v);
    else if (deg[w] < K) Cut(w);
    else {
      v = Contract(v, w);
      if (deg[v] < K) Cut(v);
    }
  }

  // Finalize
  for (size_t i = 0; i < vs.size(); ++i) {
    tmp_vs[UFRoot(vs[i])].push_back(vs[i]);
  }
  for (size_t i = 0; i < es.size(); ++i) {
    int r1 = UFRoot(es[i].first), r2 = UFRoot(es[i].second);
    if (r1 == r2) tmp_es[r1].push_back(es[i]);
  }
  for (size_t i = 0; i < vs.size(); ++i) {
    int v = vs[i];
    if ((int)tmp_vs[v].size() >= K) {
      comps_vs.push_back(vector<int>());
      comps_es.push_back(vector<pii>());
      comps_vs.back().swap(tmp_vs[v]);
      comps_es.back().swap(tmp_es[v]);
    }
    tmp_vs[v].clear();
    tmp_es[v].clear();
  }
}

void Internal::Decompose(const vector<pii> &es, vector<vector<int> > &mkecs_vs,
                         int k, int num_iterations) {
  if (es.empty()) return;
  K = k;

  // Prepare |vs| and |V|
  vector<int> vs(es.size() * 2);
  for (size_t i = 0; i < es.size(); ++i) {
    vs[i * 2] = es[i].first;
    vs[i * 2 + 1] = es[i].second;
  }
  sort(vs.begin(), vs.end());
  vs.erase(unique(vs.begin(), vs.end()), vs.end());
  V = vs.back() + 1;

  // Prepare arrays
  adj.resize(V);
  deg.resize(V);
  tmp_vs.resize(V);
  tmp_es.resize(V);
  kcore_adj.resize(V);
  kcore_deg.resize(V);
  uf_par.resize(V);

  mkecs_vs.assign(1, vs);
  vector<vector<pii> > mkecs_es(1, es);

  // Iterate
  for (int iteration = 0; iteration < num_iterations; ++iteration) {
    comps_vs.clear();
    comps_es.clear();
    for (size_t i = 0; i < mkecs_vs.size(); ++i) {
      DecomposeIteration(mkecs_vs[i], mkecs_es[i]);
    }
    mkecs_vs.swap(comps_vs);
    mkecs_es.swap(comps_es);
  }
}

void Decompose(const vector<pii> &edges, vector<vector<int> > &mkecs,
               int k, int num_iterations) {
  Internal().Decompose(edges, mkecs, k, num_iterations);
}
}
