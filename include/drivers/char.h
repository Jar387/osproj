#define CDEV_VGA 0

int creadc(int dev, char* buf);
int cwritec(int dev, char buf);
int cread(int dev, char* buf, int count);
int cwrite(int dev, char* buf, int count);