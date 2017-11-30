#ifndef INC_LIFEGAME_HPP_
#define INC_LIFEGAME_HPP_

#define BUF(x, y) (buffer[((x) * N + (y)) * 4 + 1] > 0 ? 1 : 0) 
#define ALIVE (0xFF)
#define DEAD  (0)


struct LifeGame {
  explicit LifeGame(const int n): N(n), size(n - 2) {
    buffer = (GLubyte*)calloc(N * N * 4, sizeof(GLubyte));
  }
  ~LifeGame() {
    free(buffer);
  }
  void born(int x, int y) {
    buffer[((x + 1) * (size + 2) + (y + 1)) * 4 + 1] = ALIVE;
  }
  void die(int x, int y) {
    buffer[((x + 1) * (size * 2) + (y + 1)) * 4 + 1] = DEAD;
  }
  void step() {
    for(int i = 1; i <= size; ++i) {
      for(int k = 1; k <= size; ++k) {
        int alive = BUF(i-1, k) + BUF(i+1, k)
          + BUF(i-1, k-1) + BUF(i, k-1) + BUF(i+1, k-1)
          + BUF(i-1, k+1) + BUF(i, k+1) + BUF(i+1, k+1);
        
        if(BUF(i, k)) {
          if(2 <= alive && alive <= 3) {
            buffer[(i * N + k) * 4] = ALIVE;
          }
        } else {
          if(alive == 3) {
            buffer[(i * N + k) * 4] = ALIVE;
          }
        }
      }    
    }
    for(int i = 0; i < N; ++i) {
      for(int k = 0; k < N; ++k) {
        if(buffer[(i * N + k) * 4] > 0) {
          buffer[(i * N + k) * 4 + 1] = ALIVE;
        } else {
          buffer[(i * N + k) * 4 + 1] = DEAD;
        }
        buffer[(i * N + k) * 4] = DEAD;
      }    
    }
  }
  const int size;
  const int N;
  GLubyte* buffer;
};

#endif 
