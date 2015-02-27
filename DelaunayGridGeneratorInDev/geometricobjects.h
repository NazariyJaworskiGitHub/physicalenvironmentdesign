#ifndef GEOMETRICOBJECTS_H
#define GEOMETRICOBJECTS_H

#include <MathUtils>

namespace DelaunayGridGenerator
{
    /// Utility for making FEM:Node3D set, located on a sphere at PLC
    namespace GeometricObjects
    {
        /// See http://en.wikipedia.org/wiki/Regular_Icosahedron_t
        template <typename _NodeType_, typename _DimType_>
        class Icosahedron_t
        {
            /// For inner usage only
            private: class _Triangle
            {
                public : _NodeType_ *a = nullptr;
                public : _NodeType_ *b = nullptr;
                public : _NodeType_ *c = nullptr;
                private: int _curIndex = 0;
                public : _Triangle *neighbors[3];
                public : bool isSplitted = false;
                public : _Triangle *children[4];
                public : _NodeType_ *ab = nullptr;
                public : _NodeType_ *ac = nullptr;
                public : _NodeType_ *bc = nullptr;
                public : void setNeighbor(_Triangle * newNeighbor) noexcept
                {
                    neighbors[_curIndex] = newNeighbor;
                    newNeighbor->neighbors[newNeighbor->_curIndex] = this;
                    ++_curIndex;
                    ++(newNeighbor->_curIndex);
                }
                public : bool contains(_NodeType_ *target) const noexcept
                {
                    if(a == target)
                        return true;
                    if(b == target)
                        return true;
                    if(c == target)
                        return true;
                    return false;
                }
                public : _Triangle * getNeighbor(_NodeType_ *f, _NodeType_ *s) noexcept
                {
                    if(neighbors[0]->contains(f) && neighbors[0]->contains(s))
                        return neighbors[0];
                    if(neighbors[1]->contains(f) && neighbors[1]->contains(s))
                        return neighbors[1];
                    if(neighbors[2]->contains(f) && neighbors[2]->contains(s))
                        return neighbors[2];
                    return nullptr;
                }
                public : _NodeType_* getMidNode(_NodeType_ *f, _NodeType_ *s)
                {
                    if((a == f && b ==s) || (a == s && b ==f))
                        return ab;
                    if((a == f && c ==s) || (a == s && c ==f))
                        return ac;
                    if((b == f && c ==s) || (b == s && c ==f))
                        return bc;
                    return nullptr;
                }

                public : _Triangle(
                        _NodeType_ *a,
                        _NodeType_ *b,
                        _NodeType_ *c) noexcept :
                    a(a), b(b), c(c)
                {
                    neighbors[0] = nullptr;
                    neighbors[1] = nullptr;
                    neighbors[2] = nullptr;
                }
            };

            private: std::vector<_NodeType_*> _nodes;
            public : const std::vector<_NodeType_*> & getNodes() const noexcept {return _nodes;}
            private: std::vector<_Triangle*>     *_triangles = nullptr;
            private: _NodeType_ _center = _NodeType_(0.0, 0.0, 0.0);
            public : _NodeType_ getCenter() const noexcept {return _center;}
            private: _DimType_ _radius = 1.0;
            public : _DimType_ getRadius() const noexcept {return _radius;}
            private: bool _isReflectedToSphere = false;
            public : bool isReflectedToSphere() const noexcept {return _isReflectedToSphere;}
            /// Split each facet into 4;
            /// Level 0 -    12 Nodes;
            /// Level 1 -    42 Nodes;
            /// Level 2 -   162 Nodes;
            /// Level 3 -   642 Nodes;
            /// Level 4 -  2562 Nodes;
            /// Level 5 - 10242 Nodes;
            /// Level 6 - 40962 Nodes;
            /// see http://en.wikipedia.org/wiki/Geodesic_grid
            /// see http://kiwi.atmos.colostate.edu/BUGS/geodesic/
            public : void splitFacets() noexcept
            {
                std::vector<_Triangle*> *_newTriangles = new std::vector<_Triangle*>;
                for(auto tr = _triangles->begin(); tr != _triangles->end(); ++tr)
                {
                    _Triangle *_tr = *tr;
                    if(!_tr->isSplitted)
                    {
                        _NodeType_ *_ab;
                        _NodeType_ *_ac;
                        _NodeType_ *_bc;

                        // Find existing nodes or create new
                        _Triangle *_neighbor = _tr->getNeighbor(_tr->a, _tr->b);
                        if(_neighbor->isSplitted)
                            _ab = _neighbor->getMidNode(_tr->a, _tr->b);
                        else
                        {
                            _ab = new _NodeType_((*_tr->a + *_tr->b)/2.0);
                            _nodes.push_back(_ab);
                        }

                        _neighbor = _tr->getNeighbor(_tr->a, _tr->c);
                        if(_neighbor->isSplitted)
                            _ac = _neighbor->getMidNode(_tr->a, _tr->c);
                        else
                        {
                            _ac = new _NodeType_((*_tr->a + *_tr->c)/2.0);
                            _nodes.push_back(_ac);
                        }

                        _neighbor = _tr->getNeighbor(_tr->b, _tr->c);
                        if(_neighbor->isSplitted)
                            _bc = _neighbor->getMidNode(_tr->b, _tr->c);
                        else
                        {
                            _bc = new _NodeType_((*_tr->b + *_tr->c)/2.0);
                            _nodes.push_back(_bc);
                        }

                        // Create new triangles
                        _Triangle *_t1 = new _Triangle(_tr->a, _ab, _ac);
                        _Triangle *_t2 = new _Triangle(_tr->b, _ab, _bc);
                        _Triangle *_t3 = new _Triangle(_tr->c, _ac, _bc);
                        _Triangle *_t4 = new _Triangle( _ab, _ac, _bc);
                        _newTriangles->push_back(_t1);
                        _newTriangles->push_back(_t2);
                        _newTriangles->push_back(_t3);
                        _newTriangles->push_back(_t4);

                        // Update cuttent triangle
                        _tr->isSplitted = true;
                        _tr->children[0] = _t1;
                        _tr->children[1] = _t2;
                        _tr->children[2] = _t3;
                        _tr->children[3] = _t4;
                        _tr->ab = _ab;
                        _tr->ac = _ac;
                        _tr->bc = _bc;

                        // Update neighbor relations
                        _t4->setNeighbor(_t1);
                        _t4->setNeighbor(_t2);
                        _t4->setNeighbor(_t3);

                        _neighbor = _tr->getNeighbor(_tr->a, _tr->b);
                        if(_neighbor->isSplitted)
                        {
                            for(int i=0; i<3; ++i)
                            {
                                if(_neighbor->children[i]->contains(_tr->a) &&
                                        _neighbor->children[i]->contains(_ab))
                                    _t1->setNeighbor(_neighbor->children[i]);
                                else if(_neighbor->children[i]->contains(_ab) &&
                                        _neighbor->children[i]->contains(_tr->b))
                                    _t2->setNeighbor(_neighbor->children[i]);
                            }
                        }

                        _neighbor = _tr->getNeighbor(_tr->a, _tr->c);
                        if(_neighbor->isSplitted)
                        {
                            for(int i=0; i<3; ++i)
                            {
                                if(_neighbor->children[i]->contains(_tr->a) &&
                                        _neighbor->children[i]->contains(_ac))
                                    _t1->setNeighbor(_neighbor->children[i]);
                                else if(_neighbor->children[i]->contains(_ac) &&
                                        _neighbor->children[i]->contains(_tr->c))
                                    _t3->setNeighbor(_neighbor->children[i]);
                            }
                        }

                        _neighbor = _tr->getNeighbor(_tr->b, _tr->c);
                        if(_neighbor->isSplitted)
                        {
                            for(int i=0; i<3; ++i)
                            {
                                if(_neighbor->children[i]->contains(_tr->b) &&
                                        _neighbor->children[i]->contains(_bc))
                                    _t2->setNeighbor(_neighbor->children[i]);
                                else if(_neighbor->children[i]->contains(_bc) &&
                                        _neighbor->children[i]->contains(_tr->c))
                                    _t3->setNeighbor(_neighbor->children[i]);
                            }
                        }
                    }
                }
                // delete old facets
                for(auto tr : *_triangles)
                    delete tr;
                delete _triangles;
                _triangles = _newTriangles;
            }
            private: void _init() noexcept
            {
                // Construct Icosahedron_t nodes (12 nodes)
                // Note, nodes can be found as:
                // (   0.0, +/-1.0, +/-_fi)
                // (+/-1.0, +/-_fi,    0.0)
                // (+/-_fi,    0.0, +/-1.0)
                _DimType_ _fi = (1+std::sqrt(5.0))/2;

                _nodes.push_back(new _NodeType_( 0.0, -1.0, -_fi));
                _nodes.push_back(new _NodeType_( 0.0, -1.0,  _fi));
                _nodes.push_back(new _NodeType_( 0.0,  1.0, -_fi));
                _nodes.push_back(new _NodeType_( 0.0,  1.0,  _fi));

                _nodes.push_back(new _NodeType_( -1.0, -_fi, 0.0));
                _nodes.push_back(new _NodeType_( -1.0,  _fi, 0.0));
                _nodes.push_back(new _NodeType_(  1.0, -_fi, 0.0));
                _nodes.push_back(new _NodeType_(  1.0,  _fi, 0.0));

                _nodes.push_back(new _NodeType_( -_fi, 0.0, -1.0));
                _nodes.push_back(new _NodeType_(  _fi, 0.0, -1.0));
                _nodes.push_back(new _NodeType_( -_fi, 0.0,  1.0));
                _nodes.push_back(new _NodeType_(  _fi, 0.0,  1.0));

                // Construct Icosahedron_t facets (20 facets)
                // each facet is neighbor of second except last one
                _triangles = new std::vector<_Triangle*>;
                _triangles->push_back(new _Triangle( _nodes[ 0], _nodes[ 4], _nodes[ 8])); // 0
                _triangles->push_back(new _Triangle( _nodes[ 0], _nodes[ 4], _nodes[ 6])); // 1
                _triangles->push_back(new _Triangle( _nodes[ 0], _nodes[ 9], _nodes[ 6])); // 2
                _triangles->push_back(new _Triangle( _nodes[ 0], _nodes[ 9], _nodes[ 2])); // 3
                _triangles->push_back(new _Triangle( _nodes[ 0], _nodes[ 8], _nodes[ 2])); // 4

                _triangles->push_back(new _Triangle( _nodes[ 2], _nodes[ 8], _nodes[ 5])); // 5
                _triangles->push_back(new _Triangle( _nodes[10], _nodes[ 8], _nodes[ 5])); // 6
                _triangles->push_back(new _Triangle( _nodes[10], _nodes[ 8], _nodes[ 4])); // 7
                _triangles->push_back(new _Triangle( _nodes[10], _nodes[ 1], _nodes[ 4])); // 8
                _triangles->push_back(new _Triangle( _nodes[ 6], _nodes[ 1], _nodes[ 4])); // 9
                _triangles->push_back(new _Triangle( _nodes[ 6], _nodes[ 1], _nodes[11])); //10
                _triangles->push_back(new _Triangle( _nodes[ 6], _nodes[ 9], _nodes[11])); //11
                _triangles->push_back(new _Triangle( _nodes[ 7], _nodes[ 9], _nodes[11])); //12
                _triangles->push_back(new _Triangle( _nodes[ 7], _nodes[ 9], _nodes[ 2])); //13
                _triangles->push_back(new _Triangle( _nodes[ 7], _nodes[ 5], _nodes[ 2])); //14

                _triangles->push_back(new _Triangle( _nodes[ 7], _nodes[ 5], _nodes[ 3])); //15
                _triangles->push_back(new _Triangle( _nodes[10], _nodes[ 5], _nodes[ 3])); //16
                _triangles->push_back(new _Triangle( _nodes[10], _nodes[ 1], _nodes[ 3])); //17
                _triangles->push_back(new _Triangle( _nodes[11], _nodes[ 1], _nodes[ 3])); //18
                _triangles->push_back(new _Triangle( _nodes[11], _nodes[ 7], _nodes[ 3])); //19

                // Construct neighbor relations
                (*_triangles)[ 0]->setNeighbor((*_triangles)[ 1]);
                (*_triangles)[ 0]->setNeighbor((*_triangles)[ 4]);
                (*_triangles)[ 0]->setNeighbor((*_triangles)[ 7]);
                (*_triangles)[ 1]->setNeighbor((*_triangles)[ 2]);
                (*_triangles)[ 1]->setNeighbor((*_triangles)[ 9]);
                (*_triangles)[ 2]->setNeighbor((*_triangles)[ 3]);
                (*_triangles)[ 2]->setNeighbor((*_triangles)[11]);
                (*_triangles)[ 3]->setNeighbor((*_triangles)[ 4]);
                (*_triangles)[ 3]->setNeighbor((*_triangles)[13]);
                (*_triangles)[ 4]->setNeighbor((*_triangles)[ 5]);

                (*_triangles)[ 5]->setNeighbor((*_triangles)[ 6]);
                (*_triangles)[ 5]->setNeighbor((*_triangles)[14]);
                (*_triangles)[ 6]->setNeighbor((*_triangles)[ 7]);
                (*_triangles)[ 6]->setNeighbor((*_triangles)[16]);
                (*_triangles)[ 7]->setNeighbor((*_triangles)[ 8]);
                (*_triangles)[ 8]->setNeighbor((*_triangles)[ 9]);
                (*_triangles)[ 8]->setNeighbor((*_triangles)[17]);
                (*_triangles)[ 9]->setNeighbor((*_triangles)[10]);
                (*_triangles)[10]->setNeighbor((*_triangles)[11]);
                (*_triangles)[10]->setNeighbor((*_triangles)[18]);
                (*_triangles)[11]->setNeighbor((*_triangles)[12]);
                (*_triangles)[12]->setNeighbor((*_triangles)[13]);
                (*_triangles)[12]->setNeighbor((*_triangles)[19]);
                (*_triangles)[13]->setNeighbor((*_triangles)[14]);
                (*_triangles)[14]->setNeighbor((*_triangles)[15]);

                (*_triangles)[15]->setNeighbor((*_triangles)[16]);
                (*_triangles)[15]->setNeighbor((*_triangles)[19]);
                (*_triangles)[16]->setNeighbor((*_triangles)[17]);
                (*_triangles)[17]->setNeighbor((*_triangles)[18]);
                (*_triangles)[18]->setNeighbor((*_triangles)[19]);
            }

            /// newRadius should be > 0.0
            public : void scaleAndMove(
                    const _NodeType_ &newCenter,
                    _DimType_ newRadius) noexcept
            {
                for(auto i : _nodes)
                {
                    *i -= _center;
                    *i *= newRadius/_radius;
                    *i += newCenter;
                }
                _center = newCenter;
                _radius = newRadius;
            }

            /// Can be done only once
            /// see isReflectedToSphere()
            /// see http://en.wikipedia.org/wiki/Geodesic_grid
            /// see http://kiwi.atmos.colostate.edu/BUGS/geodesic/
            public : void reflectToSphere() noexcept
            {
                if(!_isReflectedToSphere)
                {
                    for(auto i : _nodes)
                    {
                        *i -= _center;
                        i->normalize();
                        *i *= _radius;
                        *i += _center;
                    }
                    _isReflectedToSphere = true;
                }
            }

            /// radius should be > 0.0
            public : Icosahedron_t(const _NodeType_ &center, _DimType_ radius)
            throw(std::logic_error) :
                _center(center), _radius(radius)
            {
                if(radius <= _DimType_(0.0))
                    throw std::logic_error("Icosahedron(): radius <= 0.0");
                _init();

                for(auto i : _nodes)
                {
                    i->normalize();
                    *i *= radius;
                    *i += center;
                }
            }

            public: ~Icosahedron_t() noexcept
            {
                for(auto tr : *_triangles)
                    delete tr;
                for(auto nod : _nodes)
                    delete nod;
            }
        };

        typedef Icosahedron_t<MathUtils::Node3D, MathUtils::Real> Icosahedron;
    }
}
#endif // GEOMETRICOBJECTS_H
