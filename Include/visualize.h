#ifndef VISUALIZE_H
#define VISUALIZE_H
int create_png(char *filename, Node *root, bool show);
int create_pdf(char *filename, Node *root, int show);
constexpr mode_t out_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
constexpr int BUFFER_SIZE = 200;
#endif
