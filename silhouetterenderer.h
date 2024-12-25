#ifndef SILHOUETTERENDERER_H
#define SILHOUETTERENDERER_H

#include <vtkSmartPointer.h>
#include <vtkPolyDataSilhouette.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

class vtkRenderer;
class vtkActor;

class SilhouetteRenderer
{
public:
    SilhouetteRenderer(vtkRenderer* renderer);
    void AddActorsWithSilhouette(const std::vector<vtkSmartPointer<vtkActor>>& actors);
    void RenderOnlySilhouette(bool=false);

private:
    vtkRenderer* Renderer;
    std::vector<vtkSmartPointer<vtkActor>> SilhouetteActors;
    std::vector<vtkSmartPointer<vtkActor>> OriginalActors;  // 保存实体模型
};

#endif // SILHOUETTERENDERER_H
