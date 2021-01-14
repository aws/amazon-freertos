/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-I11
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class ti_posix
{
    static final String VERS = "@(#) xdc-I11\n";

    static final Proto.Elm $$T_Bool = Proto.Elm.newBool();
    static final Proto.Elm $$T_Num = Proto.Elm.newNum();
    static final Proto.Elm $$T_Str = Proto.Elm.newStr();
    static final Proto.Elm $$T_Obj = Proto.Elm.newObj();

    static final Proto.Fxn $$T_Met = new Proto.Fxn(null, null, 0, -1, false);
    static final Proto.Map $$T_Map = new Proto.Map($$T_Obj);
    static final Proto.Arr $$T_Vec = new Proto.Arr($$T_Obj);

    static final XScriptO $$DEFAULT = Value.DEFAULT;
    static final Object $$UNDEF = Undefined.instance;

    static final Proto.Obj $$Package = (Proto.Obj)Global.get("$$Package");
    static final Proto.Obj $$Module = (Proto.Obj)Global.get("$$Module");
    static final Proto.Obj $$Instance = (Proto.Obj)Global.get("$$Instance");
    static final Proto.Obj $$Params = (Proto.Obj)Global.get("$$Params");

    static final Object $$objFldGet = Global.get("$$objFldGet");
    static final Object $$objFldSet = Global.get("$$objFldSet");
    static final Object $$proxyGet = Global.get("$$proxyGet");
    static final Object $$proxySet = Global.get("$$proxySet");
    static final Object $$delegGet = Global.get("$$delegGet");
    static final Object $$delegSet = Global.get("$$delegSet");

    Scriptable xdcO;
    Session ses;
    Value.Obj om;

    boolean isROV;
    boolean isCFG;

    Proto.Obj pkgP;
    Value.Obj pkgV;

    ArrayList<Object> imports = new ArrayList<Object>();
    ArrayList<Object> loggables = new ArrayList<Object>();
    ArrayList<Object> mcfgs = new ArrayList<Object>();
    ArrayList<Object> icfgs = new ArrayList<Object>();
    ArrayList<String> inherits = new ArrayList<String>();
    ArrayList<Object> proxies = new ArrayList<Object>();
    ArrayList<Object> sizes = new ArrayList<Object>();
    ArrayList<Object> tdefs = new ArrayList<Object>();

    void $$IMPORTS()
    {
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.posix.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.posix", new Value.Obj("ti.posix", pkgP));
    }

    void Util$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.posix.Util.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.posix.Util", new Value.Obj("ti.posix.Util", po));
        pkgV.bind("Util", vo);
        // decls 
    }

    void Util$$CONSTS()
    {
        // module Util
    }

    void Util$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Util$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn Util.incPath
        fxn = (Proto.Fxn)om.bind("ti.posix.Util$$incPath", new Proto.Fxn(om.findStrict("ti.posix.Util.Module", "ti.posix"), $$T_Obj, 1, 1, false));
                fxn.addArg(0, "targetName", $$T_Str, $$UNDEF);
    }

    void Util$$SIZES()
    {
    }

    void Util$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/posix/Util.xs");
        om.bind("ti.posix.Util$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.posix.Util.Module", "ti.posix");
        po.init("ti.posix.Util.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.posix.Util$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.posix.Util$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.posix.Util$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                po.addFxn("incPath", (Proto.Fxn)om.findStrict("ti.posix.Util$$incPath", "ti.posix"), Global.get(cap, "incPath"));
    }

    void Util$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.posix.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.posix"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.posix", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.posix");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.posix.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.posix'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.posix$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.posix$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.posix$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Util$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.posix.Util", "ti.posix");
        po = (Proto.Obj)om.findStrict("ti.posix.Util.Module", "ti.posix");
        vo.init2(po, "ti.posix.Util", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.posix.Util$$capsule", "ti.posix"));
        vo.bind("$package", om.findStrict("ti.posix", "ti.posix"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.posix")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.posix.Util$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("Util", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Util");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.posix.Util", "ti.posix"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.posix.Util")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.posix")).add(pkgV);
    }

    public void exec( Scriptable xdcO, Session ses )
    {
        this.xdcO = xdcO;
        this.ses = ses;
        om = (Value.Obj)xdcO.get("om", null);

        Object o = om.geto("$name");
        String s = o instanceof String ? (String)o : null;
        isCFG = s != null && s.equals("cfg");
        isROV = s != null && s.equals("rov");

        $$IMPORTS();
        $$OBJECTS();
        Util$$OBJECTS();
        Util$$CONSTS();
        Util$$CREATES();
        Util$$FUNCTIONS();
        Util$$SIZES();
        Util$$TYPES();
        if (isROV) {
            Util$$ROV();
        }//isROV
        $$SINGLETONS();
        Util$$SINGLETONS();
        $$INITIALIZATION();
    }
}
