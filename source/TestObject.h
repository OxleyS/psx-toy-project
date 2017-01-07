#ifndef _TESTOBJECT_H_
#define _TESTOBJECT_H_

#include "Global.h"
#include "GameObject.h"

class TestObject : public GameObject
{
    public:

    TestObject(const char* pMeshName = "YORDOOD.OXM");
};

#endif
