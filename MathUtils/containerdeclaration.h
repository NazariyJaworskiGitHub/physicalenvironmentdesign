/// \author Nazariy Jaworski (c)
/// \date 03.03.2015

#ifndef CONTAINERDECLARATION_H
#define CONTAINERDECLARATION_H

/// \todo it should be global include, not only for mathutils
/// try to put this into *.pro file

#ifdef _USE_QT_CONTAINERS
    /// Note, that QList is not the "real" list with O(1) insertion complexyty, when
    /// QLinkedList is. Index-search for QList or QVector is O(1) and for QLinkedList is O(N),
    /// that's why one need to use wrapped objects with self-pointers to achieve O(1) in
    /// all cases. See help references or
    /// http://qt-project.org/doc/qt-5.1/qtcore/containers.html#algorithmic-complexity

    #include <QVector>
    #include <QLinkedList>

    #define DefinedVectorType   QVector
    #define DefinedListType     QLinkedList
    #define DefinedIteratorType Iterator
#else
    #include <vector>
    #include <list>

    #define DefinedVectorType   std::vector
    #define DefinedListType     std::list
    #define DefinedIteratorType iterator
#endif

#endif // CONTAINERDECLARATION_H
