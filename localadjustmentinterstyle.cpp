#include "LocalAdjustmentInterStyle.h"
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCoordinate.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include "vtkRendererCollection.h"
#include "vtkPropPicker.h"
#include "vtkProperty.h"
#include "vtkKdTreePointLocator.h"
#include "vtkCellPicker.h"
#include "vtkDataSetSurfaceFilter.h"
#include <CGAL/Surface_mesh_deformation.h>
#include "vtkPlane.h"
#include "vtkThinPlateSplineTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyDataNormals.h"
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include "vtkTransformFilter.h"
#include "vtkTransform.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include<vtkSmoothPolyDataFilter.h>
#include<vtkWindowedSincPolyDataFilter.h>
double sign(double x1, double y1, double x2, double y2, double x3, double y3) {
    return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
}

// 判断点是否在三角形内
bool isPointInTriangle2D(double px, double py,
                         double ax, double ay,
                         double bx, double by,
                         double cx, double cy) {
    double d1 = sign(px, py, ax, ay, bx, by);
    double d2 = sign(px, py, bx, by, cx, cy);
    double d3 = sign(px, py, cx, cy, ax, ay);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

vtkStandardNewMacro(LocalAdjustmentInterStyle);
LocalAdjustmentInterStyle::LocalAdjustmentInterStyle() {

}
LocalAdjustmentInterStyle::~LocalAdjustmentInterStyle() {

}
void LocalAdjustmentInterStyle::setqtObject(MyQtObject* object)
{
    this->myqtObject = object;
}

void LocalAdjustmentInterStyle::OnLeftButtonDown() {


    this->interactor = this->GetInteractor();
    if (!interactor) {
        return;
    }
    this->renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    if (!renderer) {
        return;
    }
    int* clickPos = this->interactor->GetInteractorStyle()->GetInteractor()->GetLastEventPosition();
    vtkSmartPointer<vtkPropPicker> picker = vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, renderer);
    selectedActor = picker->GetActor();
    vtkSmartPointer<vtkCellPicker> cellPicker = vtkSmartPointer<vtkCellPicker>::New();
    cellPicker->Pick(clickPos[0], clickPos[1], 0, renderer);
    pointId = cellPicker->GetPointId();



    if(selectedActor==nullptr){
        return;
    }


    isLeftDown  = true;
    polydata = vtkPolyData::SafeDownCast(selectedActor->GetMapper()->GetInput());
    picker->Pick( clickPos[0], clickPos[1], 0, this->renderer);
    picker->GetPickPosition(selectedPos);

    // 在开始形变前，将当前的 polydata 深拷贝并压入栈中
    // vtkSmartPointer<vtkPolyData> polydataCopy = vtkSmartPointer<vtkPolyData>::New();
    // polydataCopy->DeepCopy(polydata);
    // polyDataStack.push(polydataCopy);

    // 在开始形变前，将当前的 polydata 深拷贝并压入对应 actor 的栈中
    vtkSmartPointer<vtkPolyData> polydataCopy = vtkSmartPointer<vtkPolyData>::New();
    polydataCopy->DeepCopy(polydata);
    actorPolyDataStacks[selectedActor].push(polydataCopy);

    // 如果栈大小超过最大容量，移除最早的状态
    // if (polyDataStack.size() > MAX_STACK_SIZE) {
    //     std::stack<vtkSmartPointer<vtkPolyData>> tempStack1;
    //     // 将最新的 MAX_STACK_SIZE 个元素转移到临时栈中
    //    for(int i =0; i< MAX_STACK_SIZE;i++) {
    //         tempStack1.push(polyDataStack.top());
    //         polyDataStack.pop();
    //     }
    //    std::stack<vtkSmartPointer<vtkPolyData>> tempStack2;
    //    for(int i =0; i< MAX_STACK_SIZE;i++) {
    //        tempStack2.push(tempStack1.top());
    //        tempStack1.pop();
    //    }

    //     // 交换栈
    //     std::swap(polyDataStack, tempStack2);
    // }

    vtkSmartPointer<vtkPoints> worldPoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCoordinate> coord = vtkSmartPointer<vtkCoordinate>::New();
    coord->SetCoordinateSystemToDisplay();
    coord->SetValue(clickPos[0],clickPos[1]);
    double* worldCoord = coord->GetComputedWorldValue(this->renderer);
    double bounds[6];
    this->selectedActor->GetBounds(bounds);
    double xCenter = (bounds[0] + bounds[1]) / 2.0;
    double yCenter = (bounds[2] + bounds[3]) / 2.0;
    double zCenter = (bounds[4] + bounds[5]) / 2.0;
    plane = vtkSmartPointer<vtkPlane>::New();
    double o[3] = {xCenter,yCenter,zCenter};
    plane->SetOrigin(o);
    // double *v = renderer->GetActiveCamera()->GetViewPlaneNormal();
    // double v[3] = {0,0,1};
    // plane->SetNormal(v);
    double *v = renderer->GetActiveCamera()->GetViewPlaneNormal();
    plane->SetNormal(v);

    double* cameraPosition = this->renderer->GetActiveCamera()->GetPosition();


    double direction[3];
    for (int i = 0; i < 3; i++)
    {
        direction[i] = worldCoord[i] - cameraPosition[i];
    }
    double t;
    plane->IntersectWithLine(cameraPosition, worldCoord, t, projectedPoint);
    points = polydata->GetPoints();

    switch (model) {
    case 1:
    {
        vtkSmartPointer<vtkKdTreePointLocator> kdTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
        kdTree->SetDataSet(polydata);
        kdTree->BuildLocator();
        double radius = 3.0;

        result = vtkSmartPointer<vtkIdList>::New();
        kdTree->FindPointsWithinRadius(radius, selectedPos, result);

        vtkSmartPointer<vtkKdTreePointLocator> kdTree2 = vtkSmartPointer<vtkKdTreePointLocator>::New();
        kdTree2->SetDataSet(polydata);
        kdTree2->BuildLocator();
        result2 = vtkSmartPointer<vtkIdList>::New();
        kdTree2->FindClosestNPoints(1, selectedPos, result2);
            // kdTree2->FindPointsWithinRadius(1, selectedPos, result2);
        startPos.clear();
        for (vtkIdType i = 0; i < result2->GetNumberOfIds(); ++i) {
            double p[3];
            vtkIdType id = result2->GetId(i);
            polydata->GetPoint(id,p);
            std::vector<double>v;
            v.push_back(p[0]);
            v.push_back(p[1]);
            v.push_back(p[2]);
            startPos.push_back(v);
        }

        mesh = new SMesh();
        vtkPointSet_to_polygon_mesh(polydata,*mesh);
    }
    break;
    case 2:
    {
        p1 = {bounds[0], bounds[3], zCenter};
        p2 = {bounds[1], bounds[3], zCenter};
        p3 = {xCenter, bounds[2], zCenter};

        // Select points within a certain radius from the selected point
        double radius = 2.0; // Adjust as needed

        vtkSmartPointer<vtkKdTreePointLocator> kdTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
        kdTree->SetDataSet(polydata);
        kdTree->BuildLocator();

        result = vtkSmartPointer<vtkIdList>::New();
        kdTree->FindPointsWithinRadius(radius, selectedPos, result);

        // Store the original positions of the selected points
        startPos.clear();
        for (vtkIdType i = 0; i < result->GetNumberOfIds(); ++i) {
            vtkIdType id = result->GetId(i);
            double p[3];
            polydata->GetPoint(id, p);
            std::vector<double> v = {p[0], p[1], p[2]};
            startPos.push_back(v);
        }
    }
    break;
    case 3:
    {
        Eigen::Vector3d pa= {bounds[0],yCenter,bounds[4]};
        Eigen::Vector3d pb= {xCenter,bounds[2],bounds[4]};
        Eigen::Vector3d pc= {bounds[0],bounds[2],bounds[4]};
        vtkSmartPointer<vtkPolyData>newData = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
        result3 = vtkSmartPointer<vtkIdList>::New();
        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
            double p[3];
            points->GetPoint(i, p); // 获取点坐标
            double px = p[0];
            double py = p[1];
            bool flag = isPointInTriangle2D(px, py, pa.x(), pa.y(),pb.x(), pb.y(), pc.x(), pc.y());
            if(flag){
                result3->InsertNextId(i);
                double point[3];
                points->GetPoint(i, point);
                newPoints->InsertNextPoint(point);
            }

        }
        newData->SetPoints(newPoints);
        double bounds[6];
        newData->GetBounds(bounds);
        double xCenter = (bounds[0] + bounds[1]) / 2.0;
        double yCenter = (bounds[2] + bounds[3]) / 2.0;
        double zCenter = (bounds[4] + bounds[5]) / 2.0;
        p1= {bounds[0],bounds[3],zCenter};
        p2= {bounds[1],bounds[3],zCenter};
        p3= {xCenter,bounds[2],zCenter};


    }
    break;
    default:
        break;
    }




}
void LocalAdjustmentInterStyle::OnLeftButtonUp() {

    isLeftDown = false;

}

void LocalAdjustmentInterStyle::OnMouseMove(){
    if (isLeftDown&&pointId>=0) {

        int* clickPos = this->interactor->GetInteractorStyle()->GetInteractor()->GetLastEventPosition();
        vtkSmartPointer<vtkPoints> worldPoints = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCoordinate> coord = vtkSmartPointer<vtkCoordinate>::New();
        coord->SetCoordinateSystemToDisplay();
        coord->SetValue(clickPos[0],clickPos[1]);
        double* worldCoord = coord->GetComputedWorldValue(this->renderer);
        double cameraPosition[3];
        this->renderer->GetActiveCamera()->GetPosition(cameraPosition);
        double direction[3];
        double projectedPoint[3];
        for (int i = 0; i < 3; i++)
        {
            direction[i] = worldCoord[i] - cameraPosition[i];
        }
        double t;
        plane->IntersectWithLine(cameraPosition, worldCoord, t, projectedPoint);
        double dx = (projectedPoint[0]-this->projectedPoint[0])*0.7;
        double dy = (projectedPoint[1]-this->projectedPoint[1])*0.7;
        double dz = (projectedPoint[2]-this->projectedPoint[2])*0.7;
        double displacement[3];
        for (int i = 0; i < 3; i++) {
            displacement[i] = (projectedPoint[i] - this->projectedPoint[i]) * 0.7;
        }
        switch (model) {
        case 1:
        {
            SMesh &mesh = *this->mesh;
            std::vector<Point_3> new_position;
            for(int i=0;i<startPos.size();i++){
                Point_3 p = {startPos[i][0]+dx,startPos[i][1]+dy,startPos[i][2]};
                new_position.push_back(p);

            }
            std::vector<vertex_descriptor> target_vd2;
            for (vtkIdType i = 0; i < result2->GetNumberOfIds(); ++i) {
                vtkIdType id = result2->GetId(i);
                vertex_descriptor target = vertex_descriptor(id);
                target_vd2.push_back(target);
            }
            std::map<halfedge_descriptor, double> weight_map;
            for(halfedge_descriptor h : mesh.halfedges())
            {
                vertex_descriptor source_vertex = source(h, mesh);
                vertex_descriptor target_vertex = target(h, mesh);
                if(source_vertex==target_vd2[0]||target_vertex==target_vd2[0]){
                    weight_map[h] =8;
                }else {
                    double length = CGAL::sqrt(CGAL::squared_distance( get(CGAL::vertex_point, mesh, source(h, mesh))
                                                                      ,get(CGAL::vertex_point, mesh, target(h, mesh))));
                    double weight = 1.0 / (1.0 + length*length);
                    weight_map[h] = weight;
                }

            }
            Internal_vertex_map internal_vertex_index_map;
            Vertex_index_map vertex_index_map(internal_vertex_index_map);
            vertex_iterator vb, ve;
            std::size_t counter = 0;
            for(vertex_descriptor v : vertices(mesh)) {
                put(vertex_index_map, v, counter++);
            }
            Internal_hedge_map internal_hedge_index_map;
            Hedge_index_map hedge_index_map(internal_hedge_index_map);
            counter = 0;
            for(halfedge_descriptor h : halfedges(mesh)) {
                put(hedge_index_map, h, counter++);
            }
            mesh_deformation deform(mesh,
                                    vertex_index_map,
                                    hedge_index_map,
                                    get(CGAL::vertex_point, mesh),
                                    Weights_from_map(&weight_map));

            // CGAL::Surface_mesh_deformation<SMesh> deform(mesh);
            deform.set_sre_arap_alpha(0.02);
            // std::vector<vertex_descriptor> target_vd2;
            // for (vtkIdType i = 0; i < result2->GetNumberOfIds(); ++i) {
            //     vtkIdType id = result2->GetId(i);
            //     vertex_descriptor target = vertex_descriptor(id);
            //     target_vd2.push_back(target);
            //     deform.insert_control_vertex(target);
            // }
            for(vertex_descriptor target:target_vd2){
                deform.insert_control_vertex(target);
            }
            deform.preprocess();
            for (vtkIdType i = 0; i < result->GetNumberOfIds(); ++i) {
                vtkIdType id = result->GetId(i);
                vertex_descriptor target_vd = vertex_descriptor(id);

                deform.insert_roi_vertex(target_vd);
            }
            for(int i=0;i<startPos.size();i++){
                deform.set_target_position(target_vd2[i], new_position[i]);
                Eigen::Vector3d translation_vector(dx, dy, dz);

                //deform.translate(target_vd2[i],translation_vector);

            }
            deform.deform(20,0.0001);


            vtkSmartPointer<vtkUnstructuredGrid>ug=polygon_mesh_to_vtkUnstructured(mesh);
            outPolyData= ugToPolyData(ug);
            // 更新原始的 polydata
            polydata->DeepCopy(outPolyData);
            polydata->Modified();
            // vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper> ::New();
            // // mapper->SetInputConnection(LiverNormal->GetOutputPort());
            // mapper->SetInputData(outPolyData);
            // selectedActor->SetMapper(mapper);
        }
        break;
        case 2:
        {   vtkCamera* camera = this->renderer->GetActiveCamera();
            double viewDirection[3];
            camera->GetDirectionOfProjection(viewDirection);
            vtkMath::Normalize(viewDirection);

            double viewUp[3];
            camera->GetViewUp(viewUp);
            vtkMath::Normalize(viewUp);

            double rightVector[3];
            vtkMath::Cross(viewDirection, viewUp, rightVector);
            vtkMath::Normalize(rightVector);

            // Decompose displacement vector into camera coordinates
            double dispRight = vtkMath::Dot(displacement, rightVector);
            double dispUp = vtkMath::Dot(displacement, viewUp);
            double dispForward = vtkMath::Dot(displacement, viewDirection);

            // Reconstruct the displacement vector including the forward component
            double totalDisplacement[3];
            for (int i = 0; i < 3; i++) {
                totalDisplacement[i] = dispRight * rightVector[i] + dispUp * viewUp[i] + dispForward * viewDirection[i];
            }

            // Apply maximum displacement limit if necessary
            double maxDisplacement = 3.0;
            for (int i = 0; i < 3; i++) {
                totalDisplacement[i] = std::clamp(totalDisplacement[i], -maxDisplacement, maxDisplacement);
            }

            // Update control point p3 with the new displacement
            Eigen::Vector3d displacementVector(totalDisplacement[0], totalDisplacement[1], totalDisplacement[2]);
            Eigen::Vector3d new_p3 = p3 + displacementVector * 0.5;

            // Set up original and transformed points matrices
            Eigen::Matrix3d originalPoints;
            originalPoints.col(0) = p1;
            originalPoints.col(1) = p2;
            originalPoints.col(2) = p3;

            Eigen::Matrix3d transformedPoints;
            transformedPoints.col(0) = p1;
            transformedPoints.col(1) = p2;
            transformedPoints.col(2) = new_p3;

            Eigen::Matrix3d originalPointsInverse = originalPoints.completeOrthogonalDecomposition().pseudoInverse();
            Eigen::Matrix3d transformationMatrix = transformedPoints * originalPointsInverse;

            double sigma = 0.6;

            // Apply the transformation to the selected points
            for (vtkIdType i = 0; i < result->GetNumberOfIds(); ++i) {
                vtkIdType id = result->GetId(i);
                double p[3];
                polydata->GetPoint(id, p);

                // Compute the distance from the selected position
                double distance = sqrt(vtkMath::Distance2BetweenPoints(p, selectedPos));

                // Compute weight using Gaussian function
                double weight = exp(- (distance * distance) / (2 * sigma * sigma));

                // Ensure weight is between 0 and 1
                weight = std::clamp(weight, 0.0, 1.0);

                // Apply the transformation matrix
                Eigen::Vector3d point(p[0], p[1], p[2]);
                Eigen::Vector3d transformedPoint = transformationMatrix * point;

                // Interpolate between the original and transformed positions
                Eigen::Vector3d finalPoint = point * (1.0 - weight) + transformedPoint * weight;

                // Update the point position
                polydata->GetPoints()->SetPoint(id, finalPoint.x(), finalPoint.y(), finalPoint.z());
            }

            // double maxDisplacement = 5.0;
            // // 对位移进行限制
            // dx = std::clamp(dx, -maxDisplacement, maxDisplacement);
            // dy = std::clamp(dy, -maxDisplacement, maxDisplacement);
            // dz = std::clamp(dz, -maxDisplacement, maxDisplacement);
            // Eigen::Vector3d new_p3 = {p3.x() + dx * 0.5, p3.y() + dy * 0.5, p3.z()};

            // Eigen::Matrix3d originalPoints;
            // originalPoints.col(0) = p1;
            // originalPoints.col(1) = p2;
            // originalPoints.col(2) = p3;

            // Eigen::Matrix3d transformedPoints;
            // transformedPoints.col(0) = p1;
            // transformedPoints.col(1) = p2;
            // transformedPoints.col(2) = new_p3;

            // Eigen::Matrix3d originalPointsInverse = originalPoints.completeOrthogonalDecomposition().pseudoInverse();
            // Eigen::Matrix3d transformationMatrix = transformedPoints * originalPointsInverse;

            // double sigma = 0.5;

            // for (vtkIdType i = 0; i < result->GetNumberOfIds(); ++i) {
            //     vtkIdType id = result->GetId(i);
            //     double p[3];
            //     polydata->GetPoint(id, p);

            //     // 计算点到选取中心的距离
            //     double distance = sqrt(vtkMath::Distance2BetweenPoints(p, selectedPos));

            //     // 计算权重（使用高斯函数）
            //     double weight = exp(- (distance * distance) / (2 * sigma * sigma));

            //     // 确保权重在 0 到 1 之间
            //     weight = std::clamp(weight, 0.0, 1.0);

            //     // 应用变换矩阵
            //     Eigen::Vector3d point(p[0], p[1], p[2]);
            //     Eigen::Vector3d transformedPoint = transformationMatrix * point;

            //     // 计算最终位置，基于权重进行插值
            //     Eigen::Vector3d finalPoint = point * (1.0 - weight) + transformedPoint * weight;

            //     // 更新点的位置
            //     polydata->GetPoints()->SetPoint(id, finalPoint.x(), finalPoint.y(), finalPoint.z());
            // }

            // Update the polydata and mapper
            polydata->GetPoints()->Modified();
            selectedActor->GetMapper()->Update();

            // Optionally, perform smoothing
            vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoothFilter =
                vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
            smoothFilter->SetInputData(polydata);
            smoothFilter->SetNumberOfIterations(50); // Adjust as needed
            smoothFilter->BoundarySmoothingOff();
            smoothFilter->FeatureEdgeSmoothingOff();
            smoothFilter->SetPassBand(0.1); // Adjust as needed
            smoothFilter->NormalizeCoordinatesOn();
            smoothFilter->Update();
            // Update the actor's mapper
              //  selectedActor->GetMapper()->SetInputConnection(smoothFilter->GetOutputPort());
            polydata->DeepCopy(smoothFilter->GetOutput());
            polydata->Modified();

            // Render the changes
            interactor->GetRenderWindow()->Render();
        }
        break;
        case 3:
        {

        }
        default:
            break;
        }




        emit myqtObject->refresh();
        interactor->GetRenderWindow()->Render();
    }




}



bool LocalAdjustmentInterStyle::vtkPointSet_to_polygon_mesh(vtkPointSet *poly_data, SMesh &tmesh)
{
    typedef typename boost::property_map<SMesh, CGAL::vertex_point_t>::type VPMap;
    typedef typename boost::property_map_value<SMesh, CGAL::vertex_point_t>::type Point_3;
    typedef typename boost::graph_traits<SMesh>::vertex_descriptor vertex_descriptor;

    VPMap vpmap = get(CGAL::vertex_point, tmesh);
    // get nb of points and cells
    vtkIdType nb_points = poly_data->GetNumberOfPoints();
    vtkIdType nb_cells = poly_data->GetNumberOfCells();
    //extract points
    std::vector<vertex_descriptor> vertex_map(nb_points);
    for (vtkIdType i = 0; i<nb_points; ++i)
    {
        double coords[3];
        poly_data->GetPoint(i, coords);

        vertex_descriptor v = CGAL::add_vertex(tmesh);
        put(vpmap, v, Point_3(coords[0], coords[1], coords[2]));
        vertex_map[i]=v;
    }
    for (vtkIdType i = 0; i<nb_cells; ++i)
    {
        if(poly_data->GetCellType(i) != 5
            && poly_data->GetCellType(i) != 7
            && poly_data->GetCellType(i) != 9)
            continue;
        vtkCell* cell_ptr = poly_data->GetCell(i);

        vtkIdType nb_vertices = cell_ptr->GetNumberOfPoints();
        if (nb_vertices < 3)
            return false;
        std::vector<vertex_descriptor> vr(nb_vertices);
        for (vtkIdType k=0; k<nb_vertices; ++k)
            vr[k]=vertex_map[cell_ptr->GetPointId(k)];

        CGAL::Euler::add_face(vr, tmesh);
    }
    return true;
}


vtkSmartPointer<vtkUnstructuredGrid> LocalAdjustmentInterStyle::polygon_mesh_to_vtkUnstructured(const SMesh &pmesh)
{
    typedef typename boost::graph_traits<SMesh>::vertex_descriptor   vertex_descriptor;
    typedef typename boost::graph_traits<SMesh>::face_descriptor     face_descriptor;
    typedef typename boost::graph_traits<SMesh>::halfedge_descriptor halfedge_descriptor;

    typedef typename boost::property_map<SMesh, CGAL::vertex_point_t>::const_type VPMap;
    typedef typename boost::property_map_value<SMesh, CGAL::vertex_point_t>::type Point_3;
    VPMap vpmap = get(CGAL::vertex_point, pmesh);

    vtkPoints* const vtk_points = vtkPoints::New();
    vtkCellArray* const vtk_cells = vtkCellArray::New();

    vtk_points->Allocate(CGAL::num_vertices(pmesh));
    vtk_cells->Allocate(CGAL::num_faces(pmesh));

    std::map<vertex_descriptor, vtkIdType> Vids;
    vtkIdType inum = 0;
    for(vertex_descriptor v : CGAL::vertices(pmesh))
    {
        const Point_3& p = get(vpmap, v);
        vtk_points->InsertNextPoint(CGAL::to_double(p.x()),
                                    CGAL::to_double(p.y()),
                                    CGAL::to_double(p.z()));
        Vids[v] = inum++;
    }
    for(face_descriptor f : CGAL::faces(pmesh))
    {
        vtkIdList* cell = vtkIdList::New();
        for(halfedge_descriptor h :
             CGAL::halfedges_around_face(CGAL::halfedge(f, pmesh), pmesh))
        {
            cell->InsertNextId(Vids[CGAL::target(h, pmesh)]);
        }
        vtk_cells->InsertNextCell(cell);
        cell->Delete();
    }

    vtkSmartPointer<vtkUnstructuredGrid> usg =
        vtkSmartPointer<vtkUnstructuredGrid>::New();

    usg->SetPoints(vtk_points);
    vtk_points->Delete();

    usg->SetCells(5,vtk_cells);
    vtk_cells->Delete();
    return usg;
}
vtkSmartPointer<vtkPolyData> LocalAdjustmentInterStyle::ugToPolyData(vtkSmartPointer<vtkUnstructuredGrid> ug)
{
    vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter =
        vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    surfaceFilter->SetInputData(ug);
    surfaceFilter->Update();
    return surfaceFilter->GetOutput();
}

void LocalAdjustmentInterStyle::setModel(int parameter){
    this->model = parameter;
}
void LocalAdjustmentInterStyle::OnKeyPress() {
    std::string key = this->GetInteractor()->GetKeySym();

    if (key == "z" && this->GetInteractor()->GetControlKey()) {
        // 执行撤销操作
        cout<<"ctrl+z"<<endl;

        if (selectedActor && actorPolyDataStacks.find(selectedActor) != actorPolyDataStacks.end()) {
            auto& polyDataStack = actorPolyDataStacks[selectedActor];
            if (!polyDataStack.empty()) {
                vtkSmartPointer<vtkPolyData> previousPolyData = polyDataStack.top();
                polyDataStack.pop();

                // 恢复模型状态
                if (polydata) {
                    polydata->DeepCopy(previousPolyData);
                    polydata->Modified();

                    // 更新渲染
                    selectedActor->GetMapper()->Update();
                    interactor->GetRenderWindow()->Render();
                } else {
                    std::cerr << "Error: polydata is null." << std::endl;
                }
            }
        }
    } else {
        // 调用父类方法，处理其他按键
        vtkInteractorStyleTrackballCamera::OnKeyPress();
    }
}

void LocalAdjustmentInterStyle::OnMouseWheelForward()
{
    Dolly(1.1);
    //vtkInteractorStyleTrackballCamera::OnMouseWheelForward();

    this->renderer->GetRenderWindow()->Render();
}

void LocalAdjustmentInterStyle::OnMouseWheelBackward()
{
    Dolly(0.9);

    this->renderer->GetRenderWindow()->Render();
}
void LocalAdjustmentInterStyle::Dolly(double factor)
{
    DollyToPosition(factor, this->Interactor->GetEventPosition(), renderer);

    // 更新相机裁剪范围，确保图像正确显示
    renderer->ResetCameraClippingRange();

    // 重新渲染窗口
    this->renderer->GetRenderWindow()->Render();
}
void LocalAdjustmentInterStyle::DollyToPosition(double factor, int* position, vtkRenderer* renderer)
{
    vtkCamera* cam = renderer->GetActiveCamera();
    if (cam->GetParallelProjection())
    {
        // Compute the world coordinates of the mouse position before zooming
        double mouseWorldCoordsBefore[4];
        ComputeDisplayToWorld(renderer, position[0], position[1], 0.0, mouseWorldCoordsBefore);

        // Zoom by adjusting the parallel scale
        cam->SetParallelScale(cam->GetParallelScale() / factor);

        // Compute the world coordinates of the mouse position after zooming
        double mouseWorldCoordsAfter[4];
        ComputeDisplayToWorld(renderer, position[0], position[1], 0.0, mouseWorldCoordsAfter);

        // Calculate the translation required to keep the mouse position stationary
        double delta[3];
        delta[0] = mouseWorldCoordsBefore[0] - mouseWorldCoordsAfter[0];
        delta[1] = mouseWorldCoordsBefore[1] - mouseWorldCoordsAfter[1];
        delta[2] = mouseWorldCoordsBefore[2] - mouseWorldCoordsAfter[2];

        // Adjust the camera's position and focal point
        cam->SetPosition(
            cam->GetPosition()[0] + delta[0],
            cam->GetPosition()[1] + delta[1],
            cam->GetPosition()[2] + delta[2]);

        cam->SetFocalPoint(
            cam->GetFocalPoint()[0] + delta[0],
            cam->GetFocalPoint()[1] + delta[1],
            cam->GetFocalPoint()[2] + delta[2]);
    }
    else
    {
        // 处理透视投影的情况
        double viewFocus[4], originalViewFocus[3], cameraPos[3], newCameraPos[3];
        double newFocalPoint[4], norm[3];

        // 获取相机位置和焦点
        cam->GetPosition(cameraPos);
        cam->GetFocalPoint(viewFocus);
        cam->GetFocalPoint(originalViewFocus);
        cam->GetViewPlaneNormal(norm);

        // 将焦点转换为屏幕坐标
        ComputeWorldToDisplay(renderer, viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);

        // 将鼠标位置从屏幕坐标转换为世界坐标，得到新的焦点
        ComputeDisplayToWorld(renderer, double(position[0]), double(position[1]), viewFocus[2], newFocalPoint);

        // 设置新的焦点
        cam->SetFocalPoint(newFocalPoint);

        // 缩放相机
        cam->Dolly(factor);

        // 调整相机位置，保持相机与焦点的相对位置
        cam->GetPosition(newCameraPos);

        double newPoint[3];
        newPoint[0] = originalViewFocus[0] + newCameraPos[0] - cameraPos[0];
        newPoint[1] = originalViewFocus[1] + newCameraPos[1] - cameraPos[1];
        newPoint[2] = originalViewFocus[2] ;

        cam->SetFocalPoint(newPoint);
    }
}

void LocalAdjustmentInterStyle::TranslateCamera(vtkRenderer* renderer, int fromX, int fromY, int toX, int toY)
{
    vtkCamera* cam = renderer->GetActiveCamera();
    double viewFocus[4], focalDepth;
    double newPickPoint[4], oldPickPoint[4], motionVector[3];

    // 获取焦点位置
    cam->GetFocalPoint(viewFocus);

    // 将焦点转换为屏幕坐标，获取深度值
    ComputeWorldToDisplay(renderer, viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
    focalDepth = viewFocus[2];

    // 将屏幕坐标转换为世界坐标，得到新的拾取点
    ComputeDisplayToWorld(renderer, double(toX), double(toY), focalDepth, newPickPoint);
    ComputeDisplayToWorld(renderer, double(fromX), double(fromY), focalDepth, oldPickPoint);

    // 计算移动向量
    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];

    // 更新相机位置和焦点
    cam->SetFocalPoint(
        cam->GetFocalPoint()[0] + motionVector[0],
        cam->GetFocalPoint()[1] + motionVector[1],
        cam->GetFocalPoint()[2] + motionVector[2]);

    cam->SetPosition(
        cam->GetPosition()[0] + motionVector[0],
        cam->GetPosition()[1] + motionVector[1],
        cam->GetPosition()[2] + motionVector[2]);
}

void LocalAdjustmentInterStyle::ComputeWorldToDisplay(vtkRenderer* renderer, double x, double y, double z, double displayCoords[4])
{
    renderer->SetWorldPoint(x, y, z, 1.0);
    renderer->WorldToDisplay();
    renderer->GetDisplayPoint(displayCoords);
}

void LocalAdjustmentInterStyle::ComputeDisplayToWorld(vtkRenderer* renderer, double x, double y, double z, double worldCoords[4])
{
    renderer->SetDisplayPoint(x, y, z);
    renderer->DisplayToWorld();
    renderer->GetWorldPoint(worldCoords);
    if (worldCoords[3] != 0.0)
    {
        worldCoords[0] /= worldCoords[3];
        worldCoords[1] /= worldCoords[3];
        worldCoords[2] /= worldCoords[3];
        worldCoords[3] = 1.0;
    }
}
void LocalAdjustmentInterStyle::setRender(vtkSmartPointer<vtkRenderer> render){
    this->renderer = render;
}
void LocalAdjustmentInterStyle::OnRightButtonDown() {
    renderer->ResetCamera();
    //selectedRenderer->Render();
    //generateBox2D();
    renderer->GetRenderWindow()->Render();
}
