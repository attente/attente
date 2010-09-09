class X
{
  public:

  X (int i) : x (i) {
  }

  private:

  int x;
};



typedef struct X X;



extern X* get (void);



int
main (int   argc,
      char *argv[])
{
  return 0;
}
