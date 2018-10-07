/**
 * File              : a.cpp
 * Author            : recurze
 * Date              : 17:54 07.10.2018
 * Last Modified Date: 19:56 07.10.2018
 */

#include <stdio.h>
#include <time.h>
#include <random>
#include <algorithm>

#define rep(i, n)       for(int i=0; i<n; ++i)
#define repr(i, l, r)   for(int i=l; i<r; ++i)

const int MAXN = 10101;

double processing_time;
double d[MAXN][MAXN];
double C;

int K, P, T;
int N;

int best_state[MAXN];
int current_state[MAXN];

double best_goodness = 0.0;
double current_goodness;

double goodness();
void start_state();
void update();

inline int randomint(int min, int max) {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

inline void input(){
    scanf("%lf", &processing_time);
    scanf("%d%d%d", &K, &P, &T);
    scanf("%lf", &C);

    processing_time *= 60;
    N = P * K * T;
    rep(i, N){
        rep(j, N){
            scanf("%lf", &d[i][j]);
        }
        current_state[i] = i;
    }
    current_goodness = goodness();
    update();
    start_state();
}

inline void update(){
    //current_goodness = goodness();
    if(current_goodness > best_goodness){
        rep(i, N){
            best_state[i] = current_state[i];
        }
        best_goodness = current_goodness;
    }
}

inline double goodness(){
    double ret = 0.0;
    int a, b;
    for(int t = 0; t < N; t += P * K){
        rep(i, P * K){
            a = current_state[i + t];
            repr(j, i + 1, P * K){
                b = current_state[j + t];
                if (i/K == j/K){
                    ret += 1 - d[a][b];
                } else {
                    ret += C * d[a][b];
                }
            }
        }
    }
    return ret;
}

void start_state(){
    bool alloted[N] = {0};
    double score, best_score;
    int best_candidate;

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<int> dist(0, 1);
    std::uniform_int_distribution<int> dist2(0, N-1);
    int ran;

    rep(t, T){
        ran = dist2(rng);
        while(alloted[ran]){
            ran = dist2(rng);
        }
        current_state[t * P * K] = ran;
        alloted[ran] = 1;
    }

    rep(t, T){
        repr(position, 1, P * K){
            best_score = -1e4 * 1.0;
            rep(candidate, N){
                if(!alloted[candidate]){
                    score = 0;
                    rep(k, position){
                        if(k/K == position/K){
                            score += (1-d[current_state[k + t*P*K]][candidate]);
                        } else {
                            score += C * d[current_state[k + t*P*K]][candidate];
                        }
                    }
                    if(score > best_score){
                        best_score = score;
                        best_candidate = candidate;
                    }
                }
            }
            current_state[t*P*K + position] = best_candidate;
            alloted[best_candidate] = 1;
        }
    }

    /*
    if(dist(rng)){
        rep(t, T * P){
            if(dist(rng)){
                int i = dist2(rng);
                int j = dist2(rng);
                std::swap(current_state[i], current_state[j]);
            }
        }
    }
    */

    current_goodness = goodness();
    fprintf(stderr, "beststart: %lf\n", current_goodness);
    update();
}

double swap_profit(int i, int j){
    double delta = 0.0;
    int si = i/K, sj = j/K;
    if(si == sj){
        return 0.0;
    }

    int a = current_state[i], b = current_state[j], c;

    int ti = i/(P * K), tj = j/(P * K);
    // different slot
    if(ti != tj){
        ti *= P * K;
        tj *= P * K;
        rep(x, P * K){
            c = current_state[x + ti];
            if((x + ti)/K == si){
                if(c == a){
                    continue;
                }
                delta += d[c][a] - d[c][b];
            } else {
                delta += C * (d[c][b] - d[c][a]);
            }
            c = current_state[x + tj];
            if((x + tj)/K == sj){
                if(c == b){
                    continue;
                }
                delta += d[c][b] - d[c][a];
            } else {
                delta += C * (d[c][a] - d[c][b]);
            }
        }
        return delta;
    }

    si *= K;
    sj *= K;
    rep(x, K){
        c = current_state[x + si];
        if(x + si != i) {
            delta += (C + 1) * (d[c][a] - d[c][b]);
        }
        c = current_state[x + sj];
        if(x + sj != j){
            delta += (C + 1) * (d[c][b] - d[c][a]);
        }
    }
    return delta;
}


inline double climb(){
    double improv = 0, delta;
    int end_session;
    rep(i, N){
        end_session = (i/K + 1) * K;
        repr(j, end_session, N){
            delta = swap_profit(i, j);
            if(delta > 0){
                std::swap(current_state[i], current_state[j]);
                improv += delta;
            }
        }
    }
    current_goodness += improv;
    return improv;
}

inline void hill_climb(){
    double improv = climb();
    if(improv/current_goodness < 0.001){
        update();
        int i = randomint(0, N - 1);
        int j = randomint(i, std::min(i + P * K, N) - 1);
        std::mt19937 rng;
        shuffle(current_state + i, current_state + j, rng);
        current_goodness = goodness();
    }
}

inline void print(){
    rep(t, T){
        rep(p, P - 1){
            rep(k, K){
                printf("%d ", best_state[t * P * K + p * K + k]);
            }
            printf("| ");
        }
        rep(k, K){
            printf("%d ", best_state[(t + 1) * P * K - K + k]);
        }
        printf("\n");
    }
}

clock_t start;
clock_t end;

int main(){
    start = clock();

    input();
    hill_climb();

    end = clock();
    double hc_time = double(end - start)/CLOCKS_PER_SEC;

    processing_time -= 1 + hc_time;

    double elapsed = hc_time;
    if(hc_time < 1){
        while(elapsed < processing_time){
            hill_climb();
            end = clock();
            elapsed = double(end - start)/CLOCKS_PER_SEC;
        }
    } else {
        double l = processing_time/hc_time;
        if(l - (int)l < 0.2){
            l -= 1;
        }
        rep(_, (int)l){
            hill_climb();
        }
    }

    update();
    print();
    return 0;
}
