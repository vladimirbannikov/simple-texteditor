#include "model/Model.h"
#include "controller/Controller.h"
#include "view/View.h"

int main()
{
    Model m;
    View v(&m);
    Controller c(&m);
    c.Navigation();
    return 0;
}
