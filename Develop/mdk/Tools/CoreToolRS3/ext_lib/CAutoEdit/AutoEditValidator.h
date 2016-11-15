#pragma once
#include "AutoEdit.h"

class CGeneralValidate : public CEditValidate 
{
private:
    virtual CString validate(CString& str);
};

class CColorPickerValidate : public CEditValidate 
{
private:
    virtual CString validate(CString& str);
};
