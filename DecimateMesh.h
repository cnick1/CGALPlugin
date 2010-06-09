/*
 * DecimateMesh.h
 *
 *  Created on: 2nd of June 2010
 *      Author: Olivier
 */

#ifndef CGALPLUGIN_DECIMATEMESH_H
#define CGALPLUGIN_DECIMATEMESH_H

#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/core/DataEngine.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/helper/gl/template.h>


#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>

// Adaptor for Polyhedron_3
#include <CGAL/Surface_mesh_simplification/HalfedgeGraph_Polyhedron_3.h>

// Simplification function
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>

// Stop-condition policy
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>

// Typedefs SOFA
typedef sofa::core::topology::BaseMeshTopology::Edge Edge;
typedef sofa::core::topology::BaseMeshTopology::Triangle Triangle;
typedef sofa::core::topology::BaseMeshTopology::SeqEdges SeqEdges;
typedef sofa::core::topology::BaseMeshTopology::SeqTriangles SeqTriangles;


// Typedefs CGAL
typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Surface;
typedef Surface::HalfedgeDS HalfedgeDS;
typedef Kernel::Point_3 Point;

namespace SMS = CGAL::Surface_mesh_simplification ;
using namespace sofa;
using namespace sofa::defaulttype;

namespace cgal
{

template <class DataTypes>
class DecimateMesh : public sofa::core::DataEngine
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(DecimateMesh,DataTypes),sofa::core::DataEngine);

//        typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename Coord::value_type Real;
    typedef Vec<3,Real> Vec3;

public:
    DecimateMesh();
    virtual ~DecimateMesh();

    void init();
    void reinit();

    void update();
    void draw();
    void writeObj();

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const DecimateMesh<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }

    virtual void handleEvent(sofa::core::objectmodel::Event *event);


    void geometry_to_surface(Surface &s);
    void surface_to_geometry(Surface &s);
    bool testVertexAndFindIndex(const Vec3 &vertex, int &index);

    //Inputs
    Data<VecCoord> m_inVertices;
    Data<SeqTriangles> m_inTriangles;
    Data<int> m_edgesTarget;
    Data<float> m_edgesRatio;

    // Outputs
    Data<VecCoord> m_outVertices;
    Data<SeqTriangles> m_outTriangles;


};


template <class DataTypes, class HDS>
class geometry_to_surface_op :  public CGAL::Modifier_base<HDS>
{
public:

    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::VecCoord VecCoord;

    typedef HDS Halfedge_data_structure;

private:

    VecCoord m_vertices;
    SeqTriangles m_triangles;


public:

    geometry_to_surface_op(const VecCoord &vertices, const SeqTriangles &triangles)
//        geometry_to_surface_op(helper::ReadAccessor vertices, helper::ReadAccessor triangles)
    {
        m_vertices = vertices;
        m_triangles = triangles;
    }

    void operator()( HDS& hds)
    {
        unsigned int numVertices = m_vertices.size();
        unsigned int numTriangles = m_triangles.size();

        CGAL::Polyhedron_incremental_builder_3<HalfedgeDS> builder(hds, true);
        builder.begin_surface(numVertices, numTriangles);

        for (unsigned int i = 0; i < numVertices; i++)
        {
            builder.add_vertex( Point( m_vertices[i][0], m_vertices[i][1], m_vertices[i][2] ));
        }

        for (unsigned int i = 0; i < numTriangles; i++ )
        {
            builder.begin_facet();
            for ( int j = 0; j < 3; j++ )
            {
                builder.add_vertex_to_facet( m_triangles[i][j] );
            }
            std::cout << std::endl;
            builder.end_facet();
        }

        if (builder.check_unconnected_vertices())
        {
            builder.remove_unconnected_vertices();
        }

        builder.end_surface();
    }

};


#if defined(WIN32) && !defined(CGALPLUGIN_SIMPLIFICATIONMESH_CPP)
#pragma warning(disable : 4231)
#ifndef SOFA_FLOAT
template class SOFA_CGALPLUGIN_API DecimateMesh<defaulttype::Vec3dTypes>;
#endif //SOFA_FLOAT
#ifndef SOFA_DOUBLE
template class SOFA_CGALPLUGIN_API DecimateMesh<defaulttype::Vec3fTypes>;
#endif //SOFA_DOUBLE
#endif

} //cgal

#endif /* CGALPLUGIN_DECIMATEMESH_H */
