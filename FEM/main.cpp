#include <QMap>
#include <QVector>
#include <QVector3D>

#include <iostream>
#include <cstdarg>
#include <cstring>

#include <Eigen/Dense>

#define BEAM_LENGTH         100
#define TEMPERATURE         20
#define FLUX                500
#define CONDUCTION          100

#define NUMBER_OF_ELEMENTS  4

#define MINIMAL_DISCRETIZATION_STEP 1e-10

class BoundaryCondition
{
    private: qreal _myPotential;   // I  (Neumann)
    public : const qreal & getPotential() const {return _myPotential;}
    public : void setPotential(const qreal potential) {_myPotential = potential;}
    private: qreal _myFlux;        // II (Dirichlet)
    public : const qreal & getFlux() const {return _myFlux;}
    public : void setFlux(const qreal flux) {_myFlux = flux;}
    public : BoundaryCondition(const qreal potential, const qreal flux)
    {
        _myPotential = potential;
        _myFlux = flux;
    }
};

class SimulationObject
{
    public : enum BOUNDARIES {LEFT, RIGHT};
    private: QMap<int, BoundaryCondition> _myBoundaryConditions;
    public : void setBoundaryCondition(
            const int boundaryId, const BoundaryCondition &condition)
    {
        _myBoundaryConditions.insert(boundaryId,condition);
    }
    public : const BoundaryCondition * getBoundaryCondition(const int boundaryId) const
    {
        if(_myBoundaryConditions.contains(boundaryId))
        {
            return &(*_myBoundaryConditions.find(boundaryId));
        }
        else return nullptr;
    }
    public :~SimulationObject()
    {
        _myBoundaryConditions.clear();
    }
};

/*
#include <cmath>
/// \brief The Node class provides simple support of Nodes \n
/// Node \f$ u\in E^n \f$, where \c dim is the size of coordinates
/// array, i.e. \f$ n \f$. \n
/// \todo make documentation
template <int _nDimensions_> class Node
{       
    private: double *_coordinates[_nDimensions_];
    public : double & operator [] (int index) throw(std::out_of_range)
    {
        if(index >= _nDimensions_ || index < 0)
            throw std::out_of_range("Node[i], i out of range");
        return *_coordinates[index];
    }
    public : Node(){}
    public : Node(double *ci, ...)
    {
        _coordinates[0] = ci;
        va_list _ptr;
        va_start(_ptr, ci);
        for(int i=1; i<_nDimensions_; i++)
            _coordinates[i] = va_arg(_ptr, double *);
        va_end(_ptr);
    }
    public : Node(const Node &target)
    {
        memcpy(_coordinates,target._coordinates,_nDimensions_*sizeof(double*));
    }
    public : double distanceSq( const Node &target ) const
    {
        double _dist = 0.0;
        for(int i=0;i<_nDimensions_;i++)
            _dist+=(_coordinates[i]-target._coordinates[i])*
                    (_coordinates[i]-target._coordinates[i]);
        return _dist;
    }
    public : bool equal(const Node &target) const
    {
        for(int i=0;i<_nDimensions_;i++)
            if(fabs(_coordinates[i]-target._coordinates[i])>MINIMAL_DISCRETIZATION_STEP)
                return false;
        return true;
    }
    public : bool operator == (const Node &target) const
    {
        return equal(target);
    }
    public : ~Node() {}
};*/

template <typename _NodeType_, int _nNodes_, int _nDimentions_> class FiniteElement
{
    private: _NodeType_ *_nodes[_nNodes_];
    public : _NodeType_ & operator [] (const int &index) throw(std::out_of_range)
    {
        if(index >= _nNodes_ || index < 0)
            throw std::out_of_range("FiniteElement[i], i out of range");
        return *_nodes[index];
    }
    public : FiniteElement(){}
    public : FiniteElement(_NodeType_ *ni, ...)
    {
        _nodes[0] = ni;
        va_list _ptr;
        va_start(_ptr, ni);
        for(int i=1; i<_nNodes_; i++)
            _nodes[i] = va_arg(_ptr, _NodeType_ *);
        va_end(_ptr);
    }
    public : FiniteElement(const FiniteElement &target)
    {
        memcpy(_nodes,target._nodes,_nNodes_*sizeof(_NodeType_ *));
    }
    /// \todo it fits only to simplex elements
    public :Eigen::Matrix<qreal, _nNodes_, _nNodes_> calculateStiffnessMatrix(
            const Eigen::Matrix<qreal, _nDimentions_, _nDimentions_> &conductionMatrix)
            const throw (std::logic_error)
    {
        //-Find local stiffness matrix [K], rank([K]) = number of nodes ^2:
        //
        //-Problem can be described as:
        //
        //       1                          dI(u)
        //  I(u)=-{u}^T[K]{u}-{u}^T{F}  =>  ------=[K]{u}-{F}
        //       2                          d{u}^T
        //
        //-I(u) is some complex integral function that should be approximated.
        //
        //-I(u) is approximated by sum of simple polinomial function fi:
        //  fi = a1 + a2x + ...
        //-or in the matrix form:
        //  fi = [1 x ...][a1 a2 ...]^T = [1 x ...][C]-1{u}=[N]{u}
        //
        //-where:
        //  [N] - shape function matrix,
        //
        //  [1 x ...]=[P] - polinom coefficients
        //
        //        [ 1   xi  yi ...]
        //  [C] = [ 1   xj  yj ...] rank([C]) = rank([K])
        //        [... ... ... ...]
        //
        //-To fing dI(u)/d{u}^T let's consider derivatives matrix [g]:
        //
        //        [dfi/dx]   [dNi/dx dNj/dx ...]
        //  [g] = [dfi/dy] = [dNi/dy dNj/dy ...]{u} = [B]{u}
        //        [ ...  ]   [ ...    ...   ...]
        //
        //-Then:
        //        1 /                         /
        //  I(u)= - | {u}^T[B]^T[D][B]{u}dV - |{u}^T{F}dS
        //        2 /V                        /S  (doundary conditions)
        //
        //  dI(u)                               /        /
        //  ------ = [K]{u}-{F} = [B]^T[D][B]{u}|dv - {F}|dS
        //  d{u}^T                              /V       /S
        //
        //  [K] = [B]^T[D][B]V
        //
        //        [hx 0  ...]
        //  [D] = [0  hy ...] - conduction matrix
        //        [...   ...]
        //
        //   V - volume of element, S - area of boundary with condition
        //
        //-For simplex elements
        //       |[C]|
        //   V = -----, n - number of dimensions, |.| - taking of determinant
        //         n!
        //   warning! - volume is depending of determinant sign, so renumber
        //              element's nodes if it is negative
        //
        //-Because of [N] = [1 x ...][C]-1
        //  [B] = d([1 x ...][C]^-1)/dxi = d[N]/dxi
        //  i.e [B] = [C]^-1 without first row,
        //  because polinom starts from 1, and then x, y, z, ...
        //
        //-Described method is also applicable to finding the global stiffness matrix
        //
        //-For more information, try to read:
        //  Segerlind  L 1976 Applied Finite Element Analysis
        //  (New York: Wiley, John, and Sons, Incorporated)

        Eigen::Matrix<qreal, _nNodes_, _nNodes_> _C;
        Eigen::Matrix<qreal, _nNodes_, _nNodes_> _invC;

        // calculate [C]
        for(int i=0;i<_nNodes_;i++)
        {
            _C(i,0) = 1;
            for(int j=0; j< _nDimentions_; j++)
                _C(i,j+1) = (*_nodes[i])[j];
        }

        // calculate determinant of [C]
        // calculate inversion of [C]
        bool _isInversible;
        qreal _determinant;
        /// \todo it fits only ut to 4x4 matrices
        _C.computeInverseAndDetWithCheck(_invC,_determinant,_isInversible);
        if(!_isInversible)
            throw std::logic_error("FiniteElement has zero-volume");

        // calculate volume of element V
        int _factorial = 1;
        for(int i=2; i<=_nDimentions_;i++)
            _factorial*=i;
        qreal _volume = _determinant/_factorial;

        // calculate [B]
        // tip!: it is just the references:
        auto _B =
                _invC.template block<_nNodes_ -1, _nNodes_>(1,0);
        auto _transposedB =
                _invC.transpose().template block<_nNodes_, _nNodes_-1>(0,1);

        // calculate and return [K]
        return _volume * _transposedB * conductionMatrix * _B;
    }
    public : ~FiniteElement() {}
};

/// So-called "Domain" provides the calculation of stiffness
/// matrix and force vector of specific task. \n
/// For example, for 2D simplex finite elements (triangles):\n
/// \li polynomial vector is
///     \f$ \lfloor P\rfloor = \lfloor \begin{matrix}1 & x & y\end{matrix} \rfloor \f$
/// \li shape function vector \b N is
///     \f$ \lfloor N\rfloor = \lfloor P \rfloor \left[ C\right]^{-1}, \left[ C\right]
///     =\begin{bmatrix}1 & x_i &y_i \\ 1 & x_j &y_j \\ 1 & x_k &y_k \end{bmatrix} \f$
/// \todo write all formulas
template <typename _NodeType_> class Domain
{
    private: QVector<double> _stiffnessMatrix;
    private: QVector<double> _forceVector;

    private: QVector<_NodeType_> _nodes;

    public : void createNode(_NodeType_ target)
    {
        _nodes.append(_NodeType_(target));
    }

    public : Domain(){}
    public : Domain(const Domain &target)
    {
        _stiffnessMatrix = target._stiffnessMatrix;
        _forceVector = target._forceVector;
    }
    public : ~Domain()
    {
        _stiffnessMatrix.clear();
        _forceVector.clear();
    }
};

class Node3D : public QVector3D
{
    public: Node3D(){}
    public: Node3D(qreal xp, qreal yp, qreal zp): QVector3D(xp,yp,zp){}
    public: Node3D(const Node3D &target): QVector3D(target.x(),target.y(),target.z()){}
    public: qreal operator [] (const int index) throw(std::out_of_range)
    {
        switch (index)
        {
        case 0:
            return x();
        case 1:
            return y();
        case 2:
            return z();
        default:
            throw std::out_of_range("Node3D[i], i out of range");
        }
    }
};

class Beam : public SimulationObject
{
    private: qreal _length;
    public : const qreal & getLength() const { return _length;}
    public : Beam(int length)
    {
        _length = length;
    }
};

void Test_FiniteElement()
{
    QVector3D p1(1,2,3);
    QVector3D p2(1,2,3);

    FiniteElement<QVector3D,2,3> element1(&p1, &p2);
    element1[0].setX(10);   // p1.x() == 10
    try
    {
        element1[-2].setX(555);
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    try
    {
        element1[999].setX(555);
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    FiniteElement<QVector3D,2,3> element2(element1);
    element2[0].setX(20);   // p1.x() == 20, element1[0].x() == 20

    // counterclockwise
    Node3D n1(0,0.5,0);
    Node3D n2(0,0,0);
    Node3D n3(0.5,0,0);
    FiniteElement<Node3D,3, 2> simplex(&n1, &n2, &n3);
    Eigen::Matrix<qreal, 2, 2> D;
    D << CONDUCTION,    0,
         0,             CONDUCTION;
    Eigen::Matrix<qreal, 3, 3> K = simplex.calculateStiffnessMatrix(D);
    std::cout << K << std::endl;// Should be    [ 50 -50   0]
                                //              [-50 100 -50]
                                //              [  0 -50  50]
    std::cout << "Test_FiniteElement() OK" << std::endl;
}

int main()
{
    Test_FiniteElement();

    Beam myBeam(BEAM_LENGTH);
    myBeam.setBoundaryCondition(Beam::LEFT, BoundaryCondition(TEMPERATURE,0));
    myBeam.setBoundaryCondition(Beam::RIGHT, BoundaryCondition(0,FLUX));

    Domain<Node3D> myBeamDomain;
    for(int i=0; i<NUMBER_OF_ELEMENTS; i++)
    {
        myBeamDomain.createNode(Node3D(0+i*myBeam.getLength()/NUMBER_OF_ELEMENTS,0,0));
    }
    myBeamDomain.createNode(Node3D(myBeam.getLength(),0,0));

    /*QVector<FiniteElement<Node3D,2>> elements;
    for(int i=0; i<NUMBER_OF_ELEMENTS; i++)
    {
        elements += FiniteElement<Node3D,2>(&nodes[i], &nodes[i+1]);
    }*/

    return 0;
}

