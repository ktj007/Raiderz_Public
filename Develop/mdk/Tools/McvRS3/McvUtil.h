#ifndef _McvUtil_h
#define _McvUtil_h

void GetModelTry(RLVertex* pVert,int size,DWORD color,int* face_num);
void SetLight(int mode,RVector& pos);

void DrawTry(RMatrix mat,float size,DWORD color);
void DrawMatrix(RMatrix mat,float size);
void DrawGrid(RVector pos,float w,float h,int cnt_x,int cnt_y,DWORD color);
void DrawCenter(float size_w,float size_h,DWORD color);

#endif//_McvUtil_h