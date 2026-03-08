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
        return _engine.CreateNode(type, x, y);
    }

    public void Connect(uint a, uint ac, uint b, uint bc)
    {
        _engine.Connect(a, ac, b, bc);
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
