// Oovo IV "Executioner" breakable rock fix for data\lev01\out_modelblock.bin.
// Port of Executioner-Rock-Fix.ps1 that patches the file it is given (stock GOG or rebuilt by
// SWE1R HD Redraw, which keeps the layout and only changes texture references) instead of
// always starting from the stock backup, and that can take itself out again.
//
// Three rocks in the tunnel were built into the tunnel walls' mesh group, so hitting them hides
// the wrong node (or none) and they stay solid. Each rock gets its own basic node + mesh group,
// like the track's working rocks, and its trigger points at it.
using System;

public static class RockFix {
    const int Model = 148;                                  // 0-Executioner
    const int Added = 0x64 * 3 + 0x30;                      // 3 x (basic 0x20 + group 0x44) + child list
    const int TunnelParent = 0x56378, MergedGroup = 0x563B8, OldList = 0x56394, GroupList = 0x563F4;
    const int NodeBasic = 0x5064, NodeGroup = 0x3064, Flags = 0x800AF;
    const int StockSize = 0x833C4, StockMapBytes = 0x41A0;  // model 148 in the stock file and in HD Redraw's
    static readonly int[] Keep = { 0x56414, 0x57514, 0x57E3C, 0x58738, 0x58F98 };
    static readonly int[] Mesh = { 0x59864, 0x59C50, 0x5A038 };
    static readonly int[] Trigger = { 0x5870C, 0x574E8, 0x5A460 };
    static readonly int[] OldNode = { 0, 0x517C0, 0x517C0 };

    static int BE(byte[] a, int o) { return (a[o] << 24) | (a[o + 1] << 16) | (a[o + 2] << 8) | a[o + 3]; }
    static void PutBE(byte[] a, int o, int v) { a[o] = (byte)(v >> 24); a[o + 1] = (byte)(v >> 16); a[o + 2] = (byte)(v >> 8); a[o + 3] = (byte)v; }

    class M {
        public byte[] B; public int Map, Data;
        public int Get(int o) { return BE(B, Data + o); }
        public void Put(int o, int v) { PutBE(B, Data + o, v); }
        public bool Bit(int o) { int w = o / 4; return (B[Map + w / 8] & (0x80 >> (w % 8))) != 0; }
        public void SetBit(int o, bool on) {
            int w = o / 4, i = Map + w / 8; byte m = (byte)(0x80 >> (w % 8));
            if (on) B[i] |= m; else B[i] &= (byte)~m;
        }
        public void PutPtr(int o, int v) { Put(o, v); SetBit(o, true); }
        public void Expect(int o, int v) {
            if (Get(o) != v) throw new InvalidOperationException(string.Format(
                "out_modelblock.bin isn't as expected (word at 0x{0:X} is 0x{1:X}, not 0x{2:X}).", o, Get(o), v));
        }
    }

    static M Open(byte[] b) {
        if (b.Length < 8 + 8 * (Model + 1) || BE(b, 0) != 323) throw new InvalidOperationException("out_modelblock.bin isn't the game's model file.");
        return new M { B = b, Map = BE(b, 4 + 8 * Model), Data = BE(b, 8 + 8 * Model) };
    }

    // true = fixed, false = not fixed; throws when the file is neither
    public static bool IsApplied(byte[] b) {
        var m = Open(b);
        int parentCount = m.Get(TunnelParent + 0x14);
        if (parentCount == 9 && m.Get(MergedGroup + 0x14) == 8) { CheckStock(m); return false; }
        if (parentCount == 12 && m.Get(MergedGroup + 0x14) == 5) return true;
        throw new InvalidOperationException("out_modelblock.bin has an Executioner model this installer doesn't recognise.");
    }

    static void CheckStock(M m) {
        m.Expect(TunnelParent, NodeBasic); m.Expect(TunnelParent + 4, Flags); m.Expect(TunnelParent + 8, Flags);
        m.Expect(TunnelParent + 0x14, 9); m.Expect(TunnelParent + 0x18, OldList);
        m.Expect(MergedGroup, NodeGroup); m.Expect(MergedGroup + 0x14, 8); m.Expect(MergedGroup + 0x18, GroupList);
        for (int i = 0; i < 5; i++) m.Expect(GroupList + 4 * i, Keep[i]);
        for (int i = 0; i < 3; i++) {
            m.Expect(GroupList + 4 * (5 + i), Mesh[i]);
            m.Expect(Mesh[i], 0x9E00);
            m.Expect(Trigger[i] + 0x20, OldNode[i]);
            if (((m.Get(Trigger[i] + 0x24) >> 16) & 0xFFFF) != 102) throw new InvalidOperationException("out_modelblock.bin isn't as expected (rock trigger type).");
        }
    }

    public static byte[] Apply(byte[] old) {
        var o = Open(old);
        CheckStock(o);
        int map = o.Map, dataOld = o.Data, nextOld = BE(old, 4 + 8 * (Model + 1));
        int sizeOld = nextOld - dataOld, sizeNew = sizeOld + Added;
        int mapBytesNew = (int)Math.Ceiling(sizeNew / 32.0 / 4) * 4;
        int mapGrowth = mapBytesNew - (dataOld - map);
        if (mapGrowth < 0) throw new InvalidOperationException("Unexpected offset map size.");
        int growth = mapGrowth + Added;

        var b = new byte[old.Length + growth];
        Array.Copy(old, 0, b, 0, dataOld);
        int dataNew = dataOld + mapGrowth;
        Array.Copy(old, dataOld, b, dataNew, sizeOld);
        Array.Copy(old, nextOld, b, dataNew + sizeNew, old.Length - nextOld);
        int entries = BE(old, 0) * 2 + 1;
        for (int k = 0; k < entries; k++) {
            int at = 4 + 4 * k, v = BE(old, at);
            if (v >= nextOld) PutBE(b, at, v + growth);
            else if (v == dataOld) PutBE(b, at, v + mapGrowth);
        }
        var m = new M { B = b, Map = map, Data = dataNew };

        m.Put(MergedGroup + 0x14, 5);
        for (int i = 5; i < 8; i++) { int at = GroupList + 4 * i; m.Put(at, 0); m.SetBit(at, false); }
        int pos = sizeOld;
        var nodes = new int[3];
        for (int r = 0; r < 3; r++) {
            int basic = pos, group = pos + 0x20;
            m.Put(basic, NodeBasic); m.Put(basic + 4, Flags); m.Put(basic + 8, Flags);
            m.Put(basic + 0x14, 1);
            m.PutPtr(basic + 0x18, basic + 0x1C);
            m.PutPtr(basic + 0x1C, group);
            m.Put(group, NodeGroup); m.Put(group + 4, Flags); m.Put(group + 8, Flags);
            m.Put(group + 0x14, 1);
            m.PutPtr(group + 0x18, group + 0x3C);
            for (int k = 0; k < 6; k++) m.Put(group + 0x1C + 4 * k, m.Get(Mesh[r] + 8 + 4 * k));   // bounding box
            m.PutPtr(group + 0x3C, Mesh[r]);
            m.PutPtr(Trigger[r] + 0x20, basic);
            nodes[r] = basic;
            pos += 0x64;
        }
        int list = pos;
        for (int i = 0; i < 9; i++) m.PutPtr(list + 4 * i, m.Get(OldList + 4 * i));
        for (int i = 0; i < 3; i++) m.PutPtr(list + 4 * (9 + i), nodes[i]);
        m.Put(TunnelParent + 0x14, 12);
        m.Put(TunnelParent + 0x18, list);
        return b;
    }

    // Exact inverse of Apply; the result is checked by applying the fix again and comparing.
    public static byte[] Remove(byte[] cur) {
        if (!IsApplied(cur)) return cur;
        var c = Open(cur);
        int map = c.Map, dataNew = c.Data, nextNew = BE(cur, 4 + 8 * (Model + 1));
        int sizeNew = nextNew - dataNew, sizeOld = sizeNew - Added;
        if (sizeOld != StockSize) throw new InvalidOperationException("out_modelblock.bin's Executioner model isn't the expected size.");
        int mapGrowth = (dataNew - map) - StockMapBytes;
        int growth = mapGrowth + Added, dataOld = dataNew - mapGrowth, nextOld = dataOld + sizeOld;

        var b = new byte[cur.Length - growth];
        Array.Copy(cur, 0, b, 0, dataOld);
        Array.Copy(cur, dataNew, b, dataOld, sizeOld);
        Array.Copy(cur, nextNew, b, nextOld, cur.Length - nextNew);
        int entries = BE(cur, 0) * 2 + 1;
        for (int k = 0; k < entries; k++) {
            int at = 4 + 4 * k, v = BE(cur, at);
            if (v >= nextNew) PutBE(b, at, v - growth);
            else if (v == dataNew) PutBE(b, at, v - mapGrowth);
        }
        var m = new M { B = b, Map = map, Data = dataOld };
        int list = c.Get(TunnelParent + 0x18);
        for (int i = 0; i < 9; i++) if (c.Get(list + 4 * i) != c.Get(OldList + 4 * i)) throw new InvalidOperationException("out_modelblock.bin's rock fix isn't as expected.");
        for (int o = sizeOld; o < StockMapBytes * 32; o += 4) m.SetBit(o, false);   // bits of the added words left in the kept map
        m.Put(TunnelParent + 0x14, 9);
        m.Put(TunnelParent + 0x18, OldList);
        m.Put(MergedGroup + 0x14, 8);
        for (int i = 0; i < 3; i++) {
            m.PutPtr(GroupList + 4 * (5 + i), Mesh[i]);
            m.PutPtr(Trigger[i] + 0x20, OldNode[i]);    // flagged as a pointer in the stock file even when 0
        }
        // check: applying the fix again must give back exactly the file we started from
        var again = Apply(b);
        if (again.Length != cur.Length) throw new InvalidOperationException("Taking the rock fix out didn't check out; nothing was changed.");
        for (int i = 0; i < again.Length; i++) if (again[i] != cur[i]) throw new InvalidOperationException("Taking the rock fix out didn't check out; nothing was changed.");
        return b;
    }
}
