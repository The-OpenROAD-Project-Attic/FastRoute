#ifndef __RSMT_H__
#define __RSMT_H__

namespace FastRoute {
extern int **V_table;
extern int **H_table;

extern void copyStTree(int ind, Tree rsmt);
extern void gen_brk_RSMT(Bool congestionDriven, Bool reRoute, Bool genTree, Bool newType, Bool noADJ);
extern void fluteNormal(int netID, int d, DTYPE x[], DTYPE y[], int acc, float coeffV, Tree *t);
extern void fluteCongest(int netID, int d, DTYPE x[], DTYPE y[], int acc, float coeffV, Tree *t);

}  // namespace FastRoute
#endif /* __RSMT_H__ */
