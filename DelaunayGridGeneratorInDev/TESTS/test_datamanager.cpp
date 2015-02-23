#include "test_datamanager.h"

#include <QList>
#include <QLinkedList>

#include "gridelement.h"
#include "nodewrapper.h"

using namespace DelaunayGridGenerator;

void Test_DataManager::test()
{
    QList<WrappedNode2D> _nodes = {
        MathUtils::Node2D(0,0),
        MathUtils::Node2D(0,2),
        MathUtils::Node2D(0,4),
        MathUtils::Node2D(2,0),
        MathUtils::Node2D(2,2),
        MathUtils::Node2D(2,4),
        MathUtils::Node2D(4,0),
        MathUtils::Node2D(4,2),
        MathUtils::Node2D(4,4),
        MathUtils::Node2D(1,1),
        MathUtils::Node2D(0,1)
    };

    QList<Triangle> _triangles;
    {int _indexes[] = {0,1,3}; _triangles.append(Triangle(&_nodes, _indexes));}
    {int _indexes[] = {1,3,4}; _triangles.append(Triangle(&_nodes, _indexes));}
    {int _indexes[] = {4,5,7}; _triangles.append(Triangle(&_nodes, _indexes));}
    {int _indexes[] = {5,7,8}; _triangles.append(Triangle(&_nodes, _indexes));}
    {int _indexes[] = {0,1,9}; _triangles.append(Triangle(&_nodes, _indexes));}
    {int _indexes[] = {0,9,10}; _triangles.append(Triangle(&_nodes, _indexes));}

    TreeDataManager<1, Triangle, MathUtils::Node2D, 2> _tree(
                MathUtils::Node2D(0,0), MathUtils::Node2D(4,4), &_tree);

    for(auto i = _triangles.begin(); i!= _triangles.end(); ++i)
        _tree.addDataObject(&(*i));

    QLinkedList<Triangle*>_result = _tree.getDataObjects(MathUtils::Node2D(3,1.9));
    QVERIFY(_result.size() == 4 &&
            _result.first() == &_triangles[0] &&
            _result.last() == &_triangles[3]);

    _tree.deleteDataObject(&_triangles[0]);
    _tree.deleteDataObject(&_triangles[3]);
    _result = _tree.getDataObjects(MathUtils::Node2D(3,1.9));
    QVERIFY(_result.size() == 2 &&
            _result.first() == &_triangles[1] &&
            _result.last() == &_triangles[2]);
    _tree.clear();
    _result = _tree.getDataObjects(MathUtils::Node2D(3,1.9));
    QVERIFY(_result.size() == 0);
}
