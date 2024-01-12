#define CDEV_VGA 0

int cread(int dev, char* buf, int count);
int cwrite(int dev, char* buf, int count);