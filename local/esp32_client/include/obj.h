#pragma once
#include "setup.h"
#include "core/audio/mic.h"
#include "core/audio/spk.h"
#include "core/protocol/connection.h"
#include "core/display/animation.h"

class Mic;
class Speaker;
class Connection;
class Animation;

class Object
{
public:
    static Mic *mic;
    static Speaker *spk;
    static Connection *con;
    static Animation *ani;

    static void init(Mic *m, Speaker *s, Connection *c, Animation *a)
    {
        mic = m;
        spk = s;
        con = c;
        ani = a;
    }
};
