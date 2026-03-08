using System.Windows.Forms;

namespace NodeCanvasGL.Net;

public class NodeCanvasControl : UserControl
{
    private Engine? _engine;

    public NodeCanvasControl()
    {
        _engine = new Engine();
    }

    protected override void Dispose(bool disposing)
    {
        if (disposing)
        {
            _engine?.Dispose();
            _engine = null;
        }

        base.Dispose(disposing);
    }
}
