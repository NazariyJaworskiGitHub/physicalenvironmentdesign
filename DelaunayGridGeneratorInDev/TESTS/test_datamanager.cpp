#include "test_datamanager.h"

#include "gridelement.h"
#include "nodewrapper.h"

using namespace DelaunayGridGenerator;

void Test_DataManager::test()
{
    DefinedVectorType<WrappedNode2D*> _nodes;
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(0,0)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(0,2)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(0,4)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(2,0)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(2,2)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(2,4)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(4,0)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(4,2)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(4,4)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(1,1)));
    _nodes.push_back(new WrappedNode2D(MathUtils::Node2D(0,1)));

    DefinedVectorType<Triangle*> _triangles;
    {int _indexes[] = {0,1,3}; _triangles.push_back(new Triangle(&_nodes, _indexes));}
    {int _indexes[] = {1,3,4}; _triangles.push_back(new Triangle(&_nodes, _indexes));}
    {int _indexes[] = {4,5,7}; _triangles.push_back(new Triangle(&_nodes, _indexes));}
    {int _indexes[] = {5,7,8}; _triangles.push_back(new Triangle(&_nodes, _indexes));}
    {int _indexes[] = {0,1,9}; _triangles.push_back(new Triangle(&_nodes, _indexes));}
    {int _indexes[] = {0,9,10}; _triangles.push_back(new Triangle(&_nodes, _indexes));}

    TreeDataManager<1, Triangle, MathUtils::Node2D, 2> _tree(
                MathUtils::Node2D(0,0), MathUtils::Node2D(4,4), &_tree);

    for(auto _i : _triangles)
        _tree.addDataObject(_i);

    DefinedListType<Triangle*>_result = _tree.getDataObjects(MathUtils::Node2D(3,1.9));
    QVERIFY(_result.size() == 4 &&
            _result.front() == _triangles[0] &&
            _result.back() == _triangles[3]);

    _tree.deleteDataObject(_triangles[0]);
    _tree.deleteDataObject(_triangles[3]);
    _result = _tree.getDataObjects(MathUtils::Node2D(3,1.9));
    QVERIFY(_result.size() == 2 &&
            _result.front() == _triangles[1] &&
            _result.back() == _triangles[2]);
    _tree.clear();
    _result = _tree.getDataObjects(MathUtils::Node2D(3,1.9));
    QVERIFY(_result.size() == 0);

    for(auto _i: _triangles)
        delete(_i);
    _triangles.clear();

    for(auto _i: _nodes)
        delete(_i);
    _nodes.clear();
}
