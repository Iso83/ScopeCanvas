using System.Windows.Forms;

namespace NodeCanvasGL.Net;

public class NodeCanvasControl : UserControl
{
    private readonly Engine _engine;

    public NodeCanvasControl()
    {
        _engine = new Engine();
    }

    public uint CreateNode(string type, float x, float y)
    {
        uint nodeId = _engine.CreateNode(type, x, y);
        Invalidate();
        return nodeId;
    }

    public void Connect(uint a, uint ac, uint b, uint bc)
    {
        _engine.Connect(a, ac, b, bc);
        Invalidate();
    }

    protected override void OnPaint(PaintEventArgs e)
    {
        base.OnPaint(e);
        _engine.Render();
    }

    protected override void Dispose(bool disposing)
    {
        if (disposing)
        {
            _engine.Dispose();
        }

        base.Dispose(disposing);
    }
}
