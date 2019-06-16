#ifndef _UTILITY_H_
#define _UTILITY_H_

extern void getlen();
extern void printEdge(int netID, int edgeID);
extern void plotTree(int netID);

extern void fillVIA();
extern int threeDVIA ();

extern void netpinOrderInc();

extern void writeRoute3D(char routingfile3D[]);
extern void checkRoute3D();
extern void write3D();
extern void StNetOrder();
extern Bool checkRoute3DEdgeType2(int netID, int edgeID);

extern void printTree3D(int netID);
extern void recoverEdge(int netID, int edgeID);

extern void checkUsage();
extern void netedgeOrderDec(int netID);
extern void printTree2D(int netID);
extern void finalSumCheck();
extern void ACE() ;


extern void iniBDE();
extern void copyBR (void);
extern void copyRS (void);
extern void freeRR (void);




#endif /* _UTILITY_H_ */
