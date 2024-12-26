#ifndef LOCALADJUSTMENTINTERSTYLE_H
#define LOCALADJUSTMENTINTERSTYLE_H
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include "vtkPoints.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/properties.h>
#include <CGAL/boost/graph/Euler_operations.h>
#include <vtkUnstructuredGrid.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh_deformation.h>
#include <CGAL/Polygon_mesh_processing/smooth_shape.h>
#include <stack>
#include<map>
#include<ActorWidget.h>
#include<handleinteractor.h>
const size_t MAX_STACK_SIZE = 10;
namespace PMP = CGAL::Polygon_mesh_processing;
typedef CGAL::Exact_predicates_inexact_constructions_kernel EPICK;
typedef EPICK::Point_3 Point_3;
typedef CGAL::Surface_mesh<Point_3> SMesh;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Vector_3 Vector_3;
typedef CGAL::Polyhedron_3<Kernel>     Polyhedron;
typedef boost::property_traits<boost::property_map<SMesh,CGAL::vertex_point_t>::type>::value_type DSDPoint;
// typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3> Polyhedron;
typedef boost::graph_traits<SMesh>::vertex_descriptor vertex_descriptor;
typedef CGAL::Surface_mesh_deformation<SMesh> Surface_mesh_deformation;
typedef boost::graph_traits<SMesh>::vertex_iterator vertex_iterator;

typedef boost::graph_traits<SMesh>::halfedge_descriptor halfedge_descriptor;
typedef boost::graph_traits<SMesh>::halfedge_iterator    halfedge_iterator;
typedef std::map<vertex_descriptor, std::size_t>   Internal_vertex_map;
typedef std::map<halfedge_descriptor, std::size_t>     Internal_hedge_map;
typedef boost::associative_property_map<Internal_vertex_map>   Vertex_index_map;
typedef boost::associative_property_map<Internal_hedge_map>     Hedge_index_map;

typedef boost::graph_traits<SMesh>::face_descriptor face_descriptor;
struct Weights_from_map
{
    typedef SMesh Halfedge_graph;
    Weights_from_map(std::map<halfedge_descriptor, double>* weight_map) : weight_map(weight_map)
    { }
    template<class VertexPointMap>
    double operator()(halfedge_descriptor e, SMesh& , VertexPointMap ) {
        return (*weight_map)[e];
    }
    std::map<halfedge_descriptor, double>* weight_map;
};

typedef CGAL::Surface_mesh_deformation<SMesh, Vertex_index_map, Hedge_index_map, CGAL::ORIGINAL_ARAP, Weights_from_map> mesh_deformation;
class LocalAdjustmentInterStyle : public vtkInteractorStyleTrackballCamera
{
public:

    static LocalAdjustmentInterStyle* New();
    vtkTypeMacro(LocalAdjustmentInterStyle, vtkInteractorStyleTrackballCamera);
    LocalAdjustmentInterStyle();
    ~ LocalAdjustmentInterStyle();

    virtual void OnLeftButtonDown() override;
    virtual void OnLeftButtonUp() override;
    virtual void OnMouseMove() override;
    virtual void OnKeyPress() override;
    void OnMouseWheelForward() override;
    void OnMouseWheelBackward() override;
    void OnRightButtonDown() override;
    void Dolly(double factor)override;
    void DollyToPosition(double factor, int* position, vtkRenderer* renderer);
    void TranslateCamera(vtkRenderer* renderer, int fromX, int fromY, int toX, int toY);
    void ComputeWorldToDisplay(vtkRenderer* renderer, double x, double y, double z, double displayCoords[4]);
    void ComputeDisplayToWorld(vtkRenderer* renderer, double x, double y, double z, double worldCoords[4]);
    void setModel(int parameter);
    void setqtObject(MyQtObject* object);
    void setRender(vtkSmartPointer<vtkRenderer> render);
private:
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindowInteractor> interactor;
    vtkSmartPointer<vtkActor> selectedActor;
    vtkSmartPointer<vtkPoints> points ;
    vtkSmartPointer<vtkIdList> result;
    vtkSmartPointer<vtkIdList> result2;
    vtkSmartPointer<vtkIdList> result3;
    bool isLeftDown = false;
    double selectedPos[3];
    std::vector<std::vector<double>> startPos;
    double projectedPoint[3];
    int pointId;
    SMesh* mesh;
    vtkPolyData* polydata;
    bool vtkPointSet_to_polygon_mesh(vtkPointSet *poly_data, SMesh &tmesh);
    vtkSmartPointer<vtkUnstructuredGrid> polygon_mesh_to_vtkUnstructured(const SMesh &pmesh);
    vtkSmartPointer<vtkPolyData> ugToPolyData(vtkSmartPointer<vtkUnstructuredGrid> ug);
    vtkSmartPointer<vtkPlane> plane ;
    vtkSmartPointer<vtkPolyData>outPolyData;
    int model  = 3;
    Eigen::Vector3d p1;
    Eigen::Vector3d p2;
    Eigen::Vector3d p3;
    std::stack<vtkSmartPointer<vtkPolyData>> polyDataStack;
    std::map<vtkActor*, std::stack<vtkSmartPointer<vtkPolyData>>> actorPolyDataStacks;
    MyQtObject* myqtObject;

};

#endif // LOCALADJUSTMENTINTERSTYLE_H
