#ifndef __BLOCK_H__
#define __BLOCK_H__
//TODO - ENUM THIS
#define BRICK_4x1 0
#define BRICK_4x2 1
#define BRICK_1x1 2
#define BRICK_2x1 3
#define BRICK_2x2 4
#define BRICK_6x1 5
#define BRICK_8x1 6
#define BRICK_3x2 7
#define BRICK_3x1 8
#include <vector>
extern char * BrickTypes[];
extern int BrickSizes[][2];
class LegoBlock
{
    public:
        LegoBlock(int X, int Y, int Color, int Strength, int Type, int Rotation);
        ~LegoBlock(){};
        void setPosition(int X,int Y) {x=X;y=Y;};
        void setZ(int Z) {z=Z;};
        void setDamage(int Damage) {damage=Damage;};
        bool doDamage(int amount);
        void heal(int amount) {doDamage(-amount);};
        void setType(int newtype);
        bool collideAcross(LegoBlock & other);
        void RenderShadowOnto(LegoBlock & other);
        void RenderShadowAside(LegoBlock & other);
        void stepDown() {z--;};
        int getX(){return x;};
        int getY(){return y;};
        int getZ(){return z;};
        int getType(){return type;};
        int getRotation(){return rotation;};
        int getColor(){return color;};
        int getDamage(){return damage;};
        int getStrength(){return strength;};
        double getHue(){return hue;};
        void RenderSidesBottom();
        void RenderTop();
        void RenderShadow();
        void RenderLines();
        std::vector <LegoBlock *> * getVector(){return &V;};
        unsigned long int id;
    private:
        int x,y,color,strength,type,damage,z,rotation;
        double hue;
        std::vector <LegoBlock *> V;
};
#endif
