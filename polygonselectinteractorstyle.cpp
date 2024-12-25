#include "PolygonSelectInteractorStyle.h"
#include <vtkCoordinate.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <unordered_map>
#include <set>
#include<vtkProperty.h>
#include<vtkProperty2D.h>
#include<vtkRenderWindowInteractor.h>
#include<vtkParametricSpline.h>
#include<vtkParametricFunctionSource.h>
#include<vtkPolyDataMapper.h>
#include<vtkImplicitSelectionLoop.h>
#include<vtkClipPolyData.h>
vtkStandardNewMacro(PolygonSelectInteractorStyle);

PolygonSelectInteractorStyle::PolygonSelectInteractorStyle()
    : ScreenPoints(vtkSmartPointer<vtkPoints>::New()),
    WorldPoints(vtkSmartPointer<vtkPoints>::New()),
    Renderer(nullptr),
    PolyData(nullptr),
    resultActor(vtkSmartPointer<vtkActor>::New())
    //spline(vtkSmartPointer<vtkParametricSpline>::New())
{}

void PolygonSelectInteractorStyle::OnKeyPress()
{
    std::string key = this->GetInteractor()->GetKeySym();
    if (key == "z" && this->GetInteractor()->GetControlKey())
    {
        UndoLastAction();
    }
    else
    {
        return;
    }
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void PolygonSelectInteractorStyle::OnLeftButtonDown()
{
    if (this->GetInteractor()->GetControlKey())
    {
        int* clickPos = this->GetInteractor()->GetEventPosition();

        vtkNew<vtkCoordinate> coordinate;
        coordinate->SetCoordinateSystemToDisplay();
        coordinate->SetValue(clickPos[0], clickPos[1], 0.0);

        this->ScreenPoints->InsertNextPoint(clickPos[0], clickPos[1], 0.0);
        vtkSmartPointer<vtkActor2D> pointActor = this->Add2DPointActor(clickPos);
        double worldCoords[4];
        Renderer->SetDisplayPoint(clickPos[0], clickPos[1], 0.0);
        Renderer->DisplayToWorld();
        Renderer->GetWorldPoint(worldCoords);
        if (worldCoords[3] != 0.0)
        {
            worldCoords[0] /= worldCoords[3];
            worldCoords[1] /= worldCoords[3];
            worldCoords[2] /= worldCoords[3];
            worldCoords[3] = 1.0;
        }
        WorldPoints->InsertNextPoint(worldCoords);
        vtkNew<vtkParametricSpline> spline;
        spline->SetPoints(WorldPoints);
        //spline->ClosedOn();
        vtkNew<vtkParametricFunctionSource> functionSource;
        functionSource->SetParametricFunction(spline);
        functionSource->SetUResolution(60 * (WorldPoints->GetNumberOfPoints()));
        functionSource->Update();

        vtkPolyData* out = functionSource->GetOutput();

        vtkNew<vtkPolyDataMapper> resultMapper;

        resultMapper->SetInputData(out);

        resultActor->SetMapper(resultMapper);
        resultActor->GetProperty()->SetLineWidth(2.0);  // 设置线宽为2
        resultActor->GetProperty()->SetColor(1.0, 1.0, 0.0);  // 将曲线设置为黄色
        Renderer->AddActor(resultActor);
        Renderer->GetRenderWindow()->Render();


        if (this->ScreenPoints->GetNumberOfPoints() > 1)
        {
            double p1[3];
            this->ScreenPoints->GetPoint(this->ScreenPoints->GetNumberOfPoints() - 2, p1);

            double p2[3];
            this->ScreenPoints->GetPoint(this->ScreenPoints->GetNumberOfPoints() - 1, p2);
            //vtkSmartPointer<vtkActor2D> lineActor = this->Add2DLineActor(p1, p2);
            //LineActors.push(lineActor);
        }

        PointActors.push(pointActor);
    }
    else
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
}

void PolygonSelectInteractorStyle::OnRightButtonDown()
{
    if (this->ScreenPoints->GetNumberOfPoints() > 2)
    {
        double p1[3], p2[3];
        this->ScreenPoints->GetPoint(this->ScreenPoints->GetNumberOfPoints() - 1, p1);
        this->ScreenPoints->GetPoint(0, p2);

        vtkNew<vtkParametricSpline> spline;
        spline->SetPoints(WorldPoints);
        spline->ClosedOn();
        vtkNew<vtkParametricFunctionSource> functionSource;
        functionSource->SetParametricFunction(spline);
        functionSource->SetUResolution(20 * (WorldPoints->GetNumberOfPoints()));
        functionSource->Update();

        vtkPolyData* out = functionSource->GetOutput();

        vtkNew<vtkPolyDataMapper> resultMapper;

        resultMapper->SetInputData(out);

        resultActor->SetMapper(resultMapper);
        resultActor->GetProperty()->SetLineWidth(2.0);  // 设置线宽为2
        resultActor->GetProperty()->SetColor(1.0, 1.0, 0.0);  // 将曲线设置为黄色
        Renderer->AddActor(resultActor);
        Renderer->GetRenderWindow()->Render();



        //vtkSmartPointer<vtkActor2D> lineActor = this->Add2DLineActor(p1, p2);
        //LineActors.push(lineActor);


        // 获取插值曲线上的点
        vtkPoints* curvePoints = out->GetPoints();
        vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
        ScreenPoints->Reset();
        double viewFocus[3];
        for (vtkIdType i = 0; i < out->GetNumberOfPoints()-3; i++) {
            double point[3];
            curvePoints->GetPoint(i, point);  // 获取每个点的坐标

            Renderer->SetWorldPoint(point[0], point[1], point[2], 1.0);
            Renderer->WorldToDisplay();
            Renderer->GetDisplayPoint(viewFocus);
            viewFocus[2] = 0;
            newPoints->InsertNextPoint(point);
            ScreenPoints->InsertNextPoint(viewFocus);
            std::cout << "Point " << i << ": (" << point[0] << ", " << point[1] << ", " << point[2] << ")" << std::endl;
        }




        auto start = std::chrono::high_resolution_clock::now();
        //this->PerformPolygonSelection();

        vtkSmartPointer<vtkImplicitSelectionLoop> loop = vtkSmartPointer<vtkImplicitSelectionLoop>::New();




        loop->SetLoop(newPoints);
        loop->SetAutomaticNormalGeneration(true);

        // 获取摄像机的投影方向，作为法向量
        double* normal = Renderer->GetActiveCamera()->GetDirectionOfProjection();
        loop->SetNormal(normal);

        vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
        clipper->SetInputData(PolyData);  // 使用原始 PolyData
        clipper->SetClipFunction(loop);       // 使用 vtkImplicitSelectionLoop 来裁剪
        clipper->Update();


        // 获取裁剪后的 PolyData
        vtkSmartPointer<vtkPolyData> clippedPolyData = clipper->GetOutput();


        // 更新原有 PolyData
        PolyData->DeepCopy(clippedPolyData);

        // 将裁剪后的 actor 添加到渲染器中

        Renderer->GetRenderWindow()->Render();


        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Execution time: " << diff.count() << std::endl;

        this->ClearAllPointsAndLines();

        Renderer->RemoveActor(resultActor);
        Renderer->GetRenderWindow()->Render();
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        Renderer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    }
    else
    {
        vtkInteractorStyleTrackballCamera::OnRightButtonDown();
    }
}

void PolygonSelectInteractorStyle::SetRenderer(vtkRenderer* renderer)
{
    this->Renderer = renderer;
}

void PolygonSelectInteractorStyle::SetPolyData(vtkSmartPointer<vtkPolyData> polyData)
{
    this->PolyData = polyData;
}

void PolygonSelectInteractorStyle::DrawPolyline(double* p1, double* p2)
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(p1[0], p1[1], 0.0);
    points->InsertNextPoint(p2[0], p2[1], 0.0);

    // Create a vtkCellArray to store the line (as a single polyline)
    vtkNew<vtkCellArray> lines;
    vtkNew<vtkPolyLine> polyLine;
    polyLine->GetPointIds()->SetNumberOfIds(2);  // We have 2 points in the line
    polyLine->GetPointIds()->SetId(0, 0);  // The first point
    polyLine->GetPointIds()->SetId(1, 1);  // The second point
    lines->InsertNextCell(polyLine);

    // Create a vtkPolyData to store the points and lines
    vtkNew<vtkPolyData> linePolyData;
    linePolyData->SetPoints(points);
    linePolyData->SetLines(lines);

    // Create a vtkPolyDataMapper2D to map the line data to graphics primitives
    vtkNew<vtkPolyDataMapper2D> mapper;
    mapper->SetInputData(linePolyData);

    // Create a vtkActor2D to represent the line in the scene
    vtkNew<vtkActor2D> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0.0, 1.0, 0.0);  // Set the line color to green
    actor->GetProperty()->SetLineWidth(2.0);  // Optional: set line width
    actor->GetProperty()->SetLineStipplePattern(0xF0F0); // 设置虚线模式
    actor->GetProperty()->SetLineStippleRepeatFactor(1); // 设置重复因子
    // Add the line actor to the renderer
    this->Renderer->AddActor2D(actor);
    this->GetInteractor()->GetRenderWindow()->Render();
}

void PolygonSelectInteractorStyle::PerformPolygonSelection()
{
    vtkSmartPointer<vtkPoints> points = PolyData->GetPoints();
    vtkSmartPointer<vtkCellArray> cells = PolyData->GetPolys();
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkUnsignedCharArray::SafeDownCast(PolyData->GetPointData()->GetScalars());

    vtkSmartPointer<vtkMatrix4x4> worldToProjectionMatrix = Renderer->GetActiveCamera()->GetCompositeProjectionTransformMatrix(Renderer->GetTiledAspectRatio(), 0, 1);

    int windowWidth = Renderer->GetRenderWindow()->GetSize()[0];
    int windowHeight = Renderer->GetRenderWindow()->GetSize()[1];

    if (points)
    {
        vtkIdType numPoints = points->GetNumberOfPoints();
        std::set<vtkIdType> pointsToDelete;

        vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkUnsignedCharArray> newColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        newColors->SetNumberOfComponents(3);  // Assuming RGB colors
        newColors->SetName("Colors");

        std::unordered_map<vtkIdType, vtkIdType> oldToNewIdMap;
        //#pragma omp parallel for
        // Loop through each point
        for (vtkIdType i = 0; i < numPoints; ++i)
        {
            double point[4];
            points->GetPoint(i, point);
            point[3] = 1.0;
            double projectedPoint[4];
            worldToProjectionMatrix->MultiplyPoint(point, projectedPoint);

            if (projectedPoint[3] != 0.0) {
                projectedPoint[0] /= projectedPoint[3];
                projectedPoint[1] /= projectedPoint[3];
                projectedPoint[2] /= projectedPoint[3];
            }


            projectedPoint[0] = ((projectedPoint[0] + 1.0) * windowWidth / 2.0);
            projectedPoint[1] = ((projectedPoint[1] + 1.0) * windowHeight / 2.0);
            projectedPoint[2] = 0;

            bool inside = IsPointInPolygon(projectedPoint, ScreenPoints);  // Ensure IsPointInPolygon is correctly defined
            if (!inside)
            {
                vtkIdType newId = newPoints->InsertNextPoint(point);
                oldToNewIdMap[i] = newId;

                if (colors)
                {
                    unsigned char color[3];
                    colors->GetTypedTuple(i, color);  // Use GetTypedTuple instead of GetTupleValue
                    newColors->InsertNextTypedTuple(color);  // Use InsertNextTypedTuple instead of InsertNextTupleValue
                }
            }
        }

        vtkSmartPointer<vtkCellArray> newCells = vtkSmartPointer<vtkCellArray>::New();
        vtkIdType npts;
        const vtkIdType* pts;

        cells->InitTraversal();
        while (cells->GetNextCell(npts, pts))
        {
            std::vector<vtkIdType> newCellPts;
            for (vtkIdType j = 0; j < npts; ++j)
            {
                if (oldToNewIdMap.find(pts[j]) != oldToNewIdMap.end())
                {
                    newCellPts.push_back(oldToNewIdMap[pts[j]]);
                }
            }
            if (newCellPts.size() == npts)
            {
                newCells->InsertNextCell(newCellPts.size(), newCellPts.data());
            }
        }

        // Update PolyData with the new points, cells, and color data
        vtkSmartPointer<vtkPolyData> newPolyData = vtkSmartPointer<vtkPolyData>::New();
        newPolyData->SetPoints(newPoints);
        newPolyData->SetPolys(newCells);
        if (colors)
        {
            newPolyData->GetPointData()->SetScalars(newColors);
        }

        // Replace the old PolyData
        PolyData->DeepCopy(newPolyData);
        Renderer->GetRenderWindow()->Render();
    }
    else
    {
        std::cout << "No points found in polydata." << std::endl;
    }
}

void PolygonSelectInteractorStyle::UndoLastAction()
{
    if (!PointActors.empty())
    {
        vtkSmartPointer<vtkActor2D> lastPointActor = PointActors.top();
        this->Renderer->RemoveActor(lastPointActor);
        PointActors.pop();

        if (!LineActors.empty())
        {
            vtkSmartPointer<vtkActor2D> lastLineActor = LineActors.top();
            this->Renderer->RemoveActor(lastLineActor);
            LineActors.pop();
        }

        this->ScreenPoints->SetNumberOfPoints(this->ScreenPoints->GetNumberOfPoints() - 1);
        WorldPoints->SetNumberOfPoints(this->WorldPoints->GetNumberOfPoints() - 1);
        vtkNew<vtkParametricSpline> spline;
        spline->SetPoints(WorldPoints);
        //spline->ClosedOn();
        vtkNew<vtkParametricFunctionSource> functionSource;
        functionSource->SetParametricFunction(spline);
        functionSource->SetUResolution(20 * (WorldPoints->GetNumberOfPoints()));
        functionSource->Update();

        vtkPolyData* out = functionSource->GetOutput();

        vtkNew<vtkPolyDataMapper> resultMapper;

        resultMapper->SetInputData(out);

        resultActor->SetMapper(resultMapper);
        resultActor->GetProperty()->SetLineWidth(2.0);  // 设置线宽为2
        resultActor->GetProperty()->SetColor(1.0, 1.0, 0.0);  // 将曲线设置为黄色
        Renderer->AddActor(resultActor);
        Renderer->GetRenderWindow()->Render();
        Renderer->GetRenderWindow()->Render();
    }
}

void PolygonSelectInteractorStyle::ClearAllPointsAndLines()
{
    while (!PointActors.empty())
    {
        vtkSmartPointer<vtkActor2D> pointActor = PointActors.top();
        this->Renderer->RemoveActor(pointActor);
        PointActors.pop();
    }

    while (!LineActors.empty())
    {
        vtkSmartPointer<vtkActor2D> lineActor = LineActors.top();
        this->Renderer->RemoveActor(lineActor);
        LineActors.pop();
    }

    this->ScreenPoints->Reset();
    this->WorldPoints->Reset();
    Renderer->GetRenderWindow()->Render();
}

vtkSmartPointer<vtkActor2D> PolygonSelectInteractorStyle::Add2DPointActor(int* pos)
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(pos[0], pos[1], 0.0);

    vtkNew<vtkPolyData> pointPolyData;
    pointPolyData->SetPoints(points);

    vtkNew<vtkGlyphSource2D> glyphSource;
    glyphSource->SetGlyphTypeToCircle();
    glyphSource->SetScale(5.0);  // Adjust the size of the point

    vtkNew<vtkGlyph2D> glyph;
    glyph->SetInputData(pointPolyData);
    glyph->SetSourceConnection(glyphSource->GetOutputPort());

    vtkNew<vtkPolyDataMapper2D> mapper;
    mapper->SetInputConnection(glyph->GetOutputPort());

    vtkNew<vtkActor2D> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1.0,0.0,0.0);  // Set point color to red

    this->Renderer->AddActor2D(actor);
    this->GetInteractor()->GetRenderWindow()->Render();

    return actor;
}

vtkSmartPointer<vtkActor2D> PolygonSelectInteractorStyle::Add2DLineActor(double* p1, double* p2)
{
    std::cout<<111<<std::endl;
    // Create vtkPoints object to hold the line endpoints
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(p1[0], p1[1], 0.0);
    points->InsertNextPoint(p2[0], p2[1], 0.0);

    // Create a vtkCellArray to store the line (as a single polyline)
    vtkNew<vtkCellArray> lines;
    vtkNew<vtkPolyLine> polyLine;
    polyLine->GetPointIds()->SetNumberOfIds(2);  // We have 2 points in the line
    polyLine->GetPointIds()->SetId(0, 0);  // The first point
    polyLine->GetPointIds()->SetId(1, 1);  // The second point
    lines->InsertNextCell(polyLine);

    // Create a vtkPolyData to store the points and lines
    vtkNew<vtkPolyData> linePolyData;
    linePolyData->SetPoints(points);
    linePolyData->SetLines(lines);

    // Create a vtkPolyDataMapper2D to map the line data to graphics primitives
    vtkNew<vtkPolyDataMapper2D> mapper;
    mapper->SetInputData(linePolyData);

    // Create a vtkActor2D to represent the line in the scene
    vtkNew<vtkActor2D> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0.0, 1.0, 0.0);  // Set the line color to green
    actor->GetProperty()->SetLineWidth(2.0);  // Optional: set line width

    // Add the line actor to the renderer
    this->Renderer->AddActor2D(actor);
    this->GetInteractor()->GetRenderWindow()->Render();

    return actor;
}

bool PolygonSelectInteractorStyle::IsPointInPolygon(double testPoint[3], vtkPoints* polygonPoints)
{
    // Create a polygon object
    vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
    polygon->GetPoints()->DeepCopy(polygonPoints);

    // Prepare the bounds array
    double bounds[6];
    polygon->GetBounds(bounds);

    // Compute the normal of the polygon
    double normal[3];
    polygon->ComputeNormal(
        polygon->GetPoints()->GetNumberOfPoints(),
        static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0)),
        normal);

    // Check if the point is inside the polygon
    bool inside = vtkPolygon::PointInPolygon(
        testPoint,
        polygon->GetPoints()->GetNumberOfPoints(),
        static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0)),
        bounds,
        normal);

    return inside;
}
