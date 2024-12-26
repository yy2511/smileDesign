#ifndef CGALBOOL_H
#define CGALBOOL_H
// 定义 CGAL 内核和 Surface_mesh 类型
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkCell.h>
#include <vtkIdList.h>
#include<vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/Euler_operations.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/config.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include<QString>
#include <QWidget>
#include<QFrame>
#include<QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include<QVTKOpenGLNativeWidget.h>
#include<QMessageBox>
#include<vtkTransformPolyDataFilter.h>
#include<vtkRenderer.h>
#include<vtkGenericOpenGLRenderWindow.h>
#include "CustomDlg.h"
typedef CGAL::Exact_predicates_inexact_constructions_kernel EPICK;
typedef EPICK::Point_3 Point_3;
typedef CGAL::Surface_mesh<Point_3> SMesh;
// typedef boost::property_traits<boost::property_map<SMesh, CGAL::vertex_point_t>::type>::value_type Point;
// 命名空间别名
namespace PMP = CGAL::Polygon_mesh_processing;
namespace PMP_exp = CGAL::Polygon_mesh_processing::experimental;
// 定义布尔操作类型
enum bool_op { CRF_UNION, CRF_INTER, CRF_MINUS, CRF_MINUS_OP };
namespace PMP = CGAL::Polygon_mesh_processing;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef K::Point_3 Point;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;

class STLBooleanProcessor :public QFrame{
    Q_OBJECT
public:
    bool loadFromSTL(vtkSmartPointer<vtkPolyData> m_polyData, Mesh& mesh)
    {

        std::vector<Mesh::Vertex_index> vertex_indices;
        for (vtkIdType i = 0; i < m_polyData->GetNumberOfPoints(); ++i)
        {
            double p[3];
            m_polyData->GetPoint(i, p);
            K::Point_3 point(p[0], p[1], p[2]);
            vertex_indices.push_back(mesh.add_vertex(point));
        }

        for (vtkIdType i = 0; i < m_polyData->GetNumberOfCells(); ++i)
        {
            vtkCell* cell = m_polyData->GetCell(i);
            if (cell->GetNumberOfPoints() == 3) // STL 中每个面应为三角形
            {
                vtkIdType id0 = cell->GetPointId(0);
                vtkIdType id1 = cell->GetPointId(1);
                vtkIdType id2 = cell->GetPointId(2);
                mesh.add_face(vertex_indices[id0], vertex_indices[id1], vertex_indices[id2]);
            }
        }
        return true;
    }
    bool hasSelfIntersections(const Mesh& mesh, const std::string& meshName)
    {
        bool self_intersect = PMP::does_self_intersect(mesh);
        if (self_intersect)
        {
            std::cerr << "Warning: " << meshName << " has self-intersections." << std::endl;
        }
        else
        {
            std::cout << meshName << " does not have self-intersections." << std::endl;
        }
        return self_intersect;
    }

    void fillHoles(Mesh& mesh)
    {
        for (auto halfedge : mesh.halfedges())
        {
            if (mesh.is_border(halfedge))
            {
                PMP::triangulate_hole(mesh, halfedge, CGAL::parameters::default_values());
            }
        }
    }

    template <typename Mesh>
    bool repairMesh(Mesh& mesh, const std::string& meshName) {
        namespace PMP = CGAL::Polygon_mesh_processing;
        bool success = true;

        try {
            std::cout << "开始修复网格: " << meshName << std::endl;

            // 1. 三角化所有面
            std::cout << "三角化网格..." << std::endl;
            PMP::triangulate_faces(mesh);

            // 2. 尝试缝合边界
            // std::cout << "尝试缝合边界..." << std::endl;
            // PMP::stitch_borders(mesh);

            // 3. 移除自相交
            std::cout << "检查自相交..." << std::endl;
            if (PMP::does_self_intersect(mesh)) {
                std::cout << "检测到自相交，尝试修复..." << std::endl;

                // 首先尝试使用autorefine
                PMP::experimental::autorefine_and_remove_self_intersections(mesh);

                // 如果还有自相交，尝试不保持ge  nus的修复
                if (PMP::does_self_intersect(mesh)) {
                    std::cout << "使用更激进的方法修复自相交..." << std::endl;
                    PMP::experimental::remove_self_intersections(mesh,
                                                                 CGAL::parameters::preserve_genus(false));
                }
            }

            // 4. 确保体积方向正确
            std::cout << "修正面片方向..." << std::endl;
            PMP::orient_to_bound_a_volume(mesh);

            // 最终检查
            if (PMP::does_self_intersect(mesh)) {
                std::cerr << "警告: " << meshName << " 仍然存在自相交" << std::endl;
                success = false;
            } else {
                std::cout << meshName << " 修复成功" << std::endl;
            }

        } catch (const std::exception& e) {
            std::cerr << "修复过程发生错误: " << e.what() << std::endl;
            success = false;
        }

        return success;
    }


    void removeSmallComponents(Mesh& mesh, double minVolume)
    {
        // Step 1: 计算并标记所有连接组件
        std::vector<boost::graph_traits<Mesh>::face_descriptor> component;
        std::map<boost::graph_traits<Mesh>::face_descriptor, std::size_t> face_component_map;
        std::size_t num_components = PMP::connected_components(mesh,
                                                               boost::make_assoc_property_map(face_component_map),
                                                               PMP::parameters::all_default());

        // Step 2: 计算每个组件的面积/体积
        std::vector<double> component_volumes(num_components, 0.0);
        for (auto face : mesh.faces())
        {
            std::size_t component_id = face_component_map[face];
            double face_area = PMP::face_area(face, mesh);
            component_volumes[component_id] += face_area;
        }

        // Step 3: 遍历所有组件，移除体积小于阈值的组件
        std::set<std::size_t> small_components;
        for (std::size_t i = 0; i < component_volumes.size(); ++i)
        {
            // std::cout<<"component_volumes="<<component_volumes[i]<<std::endl;
            if (component_volumes[i] < minVolume)
            {
                small_components.insert(i);
            }
        }

        std::vector<Mesh::Face_index> faces_to_remove;
        for (auto face : mesh.faces())
        {
            if (small_components.count(face_component_map[face]) > 0)
            {
                faces_to_remove.push_back(face);
            }
        }

        // 移除小组件的所有面
        for (auto face : faces_to_remove)
        {
            CGAL::Euler::remove_face(mesh.halfedge(face), mesh);
        }

        std::cout << "Small components removed based on minVolume: " << minVolume << std::endl;
    }
    bool performBooleanDifference(Mesh& mesh1, Mesh& mesh2, Mesh& result_mesh)
    {
        if(!issucessful){
            std::cout<<"fail bool"<<std::endl;
            return false;
        }
        try
        {   //removeSmallComponents(mesh1, 30.0);
            // Repair and validate input meshes
            //repairMesh(mesh1, "Mesh1");
            //repairMesh(mesh2, "Mesh2");

            // vtkSmartPointer<vtkActor> fillActor = vtkSmartPointer<vtkActor>::New();
            // saveSTL(fillActor, mesh1);
            // vtkSmartPointer<vtkActor> fillteethActor = vtkSmartPointer<vtkActor>::New();
            // saveSTL(fillteethActor, mesh2);
            // vtkSmartPointer<vtkSTLWriter> writer1 = vtkSmartPointer<vtkSTLWriter>::New();
            // writer1->SetFileName(("fill_mouth.stl"));
            // writer1->SetInputData(fillActor->GetMapper()->GetInput());
            // writer1->Write();
            // writer1->SetFileName(("fillteethActor.stl"));
            // writer1->SetInputData(fillteethActor->GetMapper()->GetInput());
            // writer1->Write();

            // Check for self-intersections before corefinement
            bool mesh1_self_intersect = hasSelfIntersections(mesh1, "Mesh1");
            bool mesh2_self_intersect = hasSelfIntersections(mesh2, "Mesh2");

            if (mesh1_self_intersect || mesh2_self_intersect)
            {
                std::cerr << "Error: One of the meshes has self-intersections. Cannot proceed with Boolean operation." << std::endl;
                //return false;
            }

            // Perform corefinement and Boolean difference
            std::cout << "Performing corefinement..." << std::endl;
            PMP::corefine(mesh1, mesh2);





            std::cout << "Performing Boolean difference..." << std::endl;
            PMP::corefine_and_compute_difference(mesh2, mesh1, result_mesh);

            // Remove small components if necessary
            removeSmallComponents(result_mesh, 30.0);

            // Validate result mesh


            return true;
        }
        catch (const CGAL::Polygon_mesh_processing::Corefinement::Self_intersection_exception& e)
        {
            std::cerr << "Error: Self-intersection detected during corefinement: " << e.what() << std::endl;
            return false;
        }

        catch (const std::exception& e)
        {
            std::cerr << "Error: Boolean difference operation failed: " << e.what() << std::endl;
            return false;
        }
    }

    void saveSTL(vtkSmartPointer<vtkActor> resultActor, const Mesh& mesh)
    {
        vtkSmartPointer<vtkPolyData> outputPolyData = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

        std::map<Mesh::Vertex_index, vtkIdType> vertex_map;
        for (auto v : mesh.vertices())
        {
            const K::Point_3& p = mesh.point(v);
            vtkIdType id = points->InsertNextPoint(p.x(), p.y(), p.z());
            vertex_map[v] = id;
        }

        outputPolyData->SetPoints(points);
        vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();
        for (auto f : mesh.faces())
        {
            std::vector<vtkIdType> ids;
            for (auto v : CGAL::vertices_around_face(mesh.halfedge(f), mesh))
            {
                ids.push_back(vertex_map[v]);
            }
            if (ids.size() == 3)
            {
                triangles->InsertNextCell(3, ids.data());
            }
        }
        outputPolyData->SetPolys(triangles);

        vtkSmartPointer<vtkPolyDataMapper> mapper3 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper3->SetInputData(outputPolyData);


        resultActor->SetMapper(mapper3);

        std::cout << "Output saved to "  << std::endl;
    }

    void setFilter(vtkSmartPointer<vtkTransformPolyDataFilter> filter){
        m_filter = filter;
    }
    void setPath(QString path){
        parentDir = path;
    }
    void setActor(vtkSmartPointer<vtkActor> resActor){
        m_resActor = resActor;
    }
    void m_render(){
        m_renderer->RemoveAllViewProps();
        m_renderer->AddActor(m_resActor);
        m_renderer->GetRenderWindow()->Render();
        show();
    }
public slots:


    // Add these slots
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();
public:
    bool issucessful = true;
    explicit STLBooleanProcessor(QWidget* parent = nullptr);
    QPushButton* saveButton;
    QPushButton* exportButton;
    QVBoxLayout* mainLayout;
    QHBoxLayout* buttonLayout;
    QVTKOpenGLNativeWidget* vtkWidget;
    vtkSmartPointer<vtkTransformPolyDataFilter> m_filter;
    QString parentDir;
    vtkSmartPointer<vtkActor> m_resActor;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
    CustomDlgTitle* m_titleBar;
};

#endif // CGALBOOL_H
