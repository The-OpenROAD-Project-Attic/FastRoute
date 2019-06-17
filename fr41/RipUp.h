#ifndef _RIPUP_H_
#define _RIPUP_H_

namespace FastRoute{

extern void ripupSegL(Segment *seg);
extern void ripupSegZ(Segment *seg);
extern void newRipup(TreeEdge* treeedge, TreeNode *treenodes, int x1, int y1, int x2, int y2);
extern Bool newRipupCheck(TreeEdge* treeedge, int x1, int y1, int x2, int y2, int ripup_threshold, int netID, int edgeID);

extern Bool newRipupType2(TreeEdge* treeedge, TreeNode *treenodes, int x1, int y1, int x2, int y2, int deg);
extern Bool newRipup3DType3(int netID, int edgeID);

extern void newRipupNet(int netID);
}

#endif /* _RIPUP_H_ */
