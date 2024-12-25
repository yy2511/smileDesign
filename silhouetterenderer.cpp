#include "SilhouetteRenderer.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkPolyDataSilhouette.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRendererCollection.h>
#include <vtkOpenGLRenderer.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkPropPicker.h>
#include <QDebug>


SilhouetteRenderer::SilhouetteRenderer(vtkRenderer* renderer)
{
    this->Renderer = renderer;
}

void SilhouetteRenderer::AddActorsWithSilhouette(const std::vector<vtkSmartPointer<vtkActor>>& actors)
{
    // 保存模型用于深度渲染
    OriginalActors = actors;

    for (auto& actor : actors)
    {
        // 创建 SilhouetteFilter 用来计算轮廓
        vtkSmartPointer<vtkPolyDataSilhouette> silhouetteFilter = vtkSmartPointer<vtkPolyDataSilhouette>::New();
        silhouetteFilter->SetInputData(actor->GetMapper()->GetInput());
        silhouetteFilter->SetCamera(this->Renderer->GetActiveCamera());
        silhouetteFilter->SetEnableFeatureAngle(1);
        silhouetteFilter->SetFeatureAngle(100);

        silhouetteFilter->Update(); // Ensure the filter is updated

        // 创建 Mapper 和 Actor 来渲染轮廓
        vtkSmartPointer<vtkPolyDataMapper> silhouetteMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        silhouetteMapper->SetInputConnection(silhouetteFilter->GetOutputPort());

        vtkSmartPointer<vtkActor> silhouetteActor = vtkSmartPointer<vtkActor>::New();
        silhouetteActor->SetMapper(silhouetteMapper);
        silhouetteActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
        silhouetteActor->GetProperty()->SetLineWidth(1.0);

        // 将轮廓 Actor 添加到 Renderer 中
        this->Renderer->AddActor(silhouetteActor);

        SilhouetteActors.push_back(silhouetteActor);
    }
}


void SilhouetteRenderer::RenderOnlySilhouette(bool onlyContour)
{   cout<<"RenderOnlySilhouette"<<endl;
    if(onlyContour)
    {
        for (auto& actor : OriginalActors)
        {
            actor->GetProperty()->SetOpacity(0.0);
        }
    }
    else{
        for (auto& actor : OriginalActors)
        {
            actor->VisibilityOn();
            actor->GetProperty()->SetOpacity(1.0);
        }
    }

    // 强制渲染更新
    Renderer->GetRenderWindow()->Render();
}
