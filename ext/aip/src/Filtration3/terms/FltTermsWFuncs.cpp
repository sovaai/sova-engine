#include <sys/types.h>
//============================================================================//
typedef struct _TableFuncEntry {
  double point;
  double value;
} TableFuncEntry;
//============================================================================//
TableFuncEntry MsgLen[] =
{
  {100,  1.0},
  {300,  1.5},
  {500,  2.0},
  {1000, 3.0},
  {5000, 4.0}
};

TableFuncEntry TermDupl[] =
{
  {1, 1.0},
  {2, 1.2},
  {3, 1.5},
  {4, 1.8},
  {5, 2.0},
  {6, 2.1},
  {7, 2.2},
  {8, 2.3},
  {9, 2.4},
  {10, 2.5}
};

/*
TableFuncEntry TermLen[] =
{
  {1, 0.7},
  {2, 2.0},
  {3, 4.0},
  {4, 6.0},
  {5, 10.0},
  {6, 12.0},
  {7, 15.0}
};
*/

/*
TableFuncEntry TermLen[] =
{
  {1, 0.7},
  {2, 1.5},
  {3, 2.0},
  {4, 2.5},
  {5, 3.0},
  {6, 3.5},
  {7, 4.0}
};
*/

TableFuncEntry TermLen[] =
{
  {1, 0.7},
  {2, 1.5},
  {3, 2.0},
  {4, 2.4},
  {5, 2.7},
  {6, 2.9},
  {7, 3.0}
};
//============================================================================//
double TableFunc( double arg, TableFuncEntry* pts,
                                  size_t pts_size )
{
  if ( arg <= pts[0].point )
    return pts[0].value;

  for ( size_t i = 1; i < pts_size; i++ )
  {
    if ( arg <   pts[i].point ) 
      return double(pts[i].value - pts[i-1].value)
             / double(pts[i].point - pts[i-1].point)
             * double(arg - pts[i-1].point)
             + pts[i-1].value;
  }

  return pts[pts_size-1].value;
}
//============================================================================//
#ifndef ARRAY_SIZE
  #define ARRAY_SIZE(Arr)           (sizeof(Arr) / sizeof (Arr[0]))
#endif
//----------------------------------------------------------------------------//
double FuncMsgLen(size_t len)
{
  return TableFunc(len, MsgLen, ARRAY_SIZE(MsgLen));
}
//----------------------------------------------------------------------------//
double FuncTermLen(size_t len)
{
  return TableFunc(len, TermLen, ARRAY_SIZE(TermLen));
}
//----------------------------------------------------------------------------//
double FuncDupl(size_t n)
{
  return TableFunc(n, TermDupl, ARRAY_SIZE(TermDupl));
}
//============================================================================//
