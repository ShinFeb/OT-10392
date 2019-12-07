import java.io.*;
import java.util.*;

//import org.apfloat.Apfloat;

public class Main {

	static final double DBL_PI = 2 * Math.PI;

	static class Point {
		double x, y;

		double dir;
		double abs;

		public Point(double x, double y) {
			this.x = x;
			this.y = y;
			dir = Math.atan2(y, x);
			abs = len();
		}

		double dot(Point p) {
			return x * p.x + y * p.y;
		}

		double cross(Point p) {
			return x * p.y - y * p.x;
		}

		double len() {
			return Math.sqrt(dot(this));
		}

		Point makeLen(double l) {
			return times(l / len());
		}

		Point times(double k) {
			return new Point(x * k, y * k);
		}

		@Override
		public String toString() {
			return "(" + x + ", " + y + "; " + dir + ", " + abs + ")";
		}

		Point to(Point p) {
			return new Point(p.x - x, p.y - y);
		}
	}

	double numSimpson(int[] xs, int[] ys, double angle, int m) {
		int n = xs.length;

		int idx = -1;
		for (int i = 0; i < n; i++) {
			int j = (i + 1) % n;
			if (ys[i] <= 0 && ys[j] > 0) {
				if (idx == -1) {
					idx = i;
				} else {
					throw new AssertionError();
				}
			}
		}

		if (idx == -1) {
			throw new AssertionError();
		}

		int nxtIdx = (idx + 1) % n;
		double nxtDir = Math.atan2(ys[nxtIdx], xs[nxtIdx]);
		while (nxtDir < 0) {
			nxtDir += 2 * Math.PI;
		}
		double normDir = Math.atan2(xs[idx] - xs[nxtIdx], ys[nxtIdx] - ys[idx]);
		double normDistSq = 0.5 * getDistSqr(xs[idx], ys[idx], xs[nxtIdx],
				ys[nxtIdx]);

		double[] arr = new double[2 * m];
		for (int i = 0; i < 2 * m; i++) {
			double dir = Math.PI / m * i;
			if (dir > nxtDir) {
				idx = (idx + 1) % n;
				nxtIdx = (idx + 1) % n;
				nxtDir = Math.atan2(ys[nxtIdx], xs[nxtIdx]);
				while (nxtDir < dir) {
					nxtDir += 2 * Math.PI;
				}
				normDir = Math
						.atan2(xs[idx] - xs[nxtIdx], ys[nxtIdx] - ys[idx]);
				normDistSq = 0.5 * getDistSqr(xs[idx], ys[idx], xs[nxtIdx],
						ys[nxtIdx]);
			}

			double cos = Math.cos(dir - normDir);
			arr[i] = normDistSq / cos / cos;
		}

		// System.err.println(Arrays.toString(arr));

		int len = (int) (angle / (2 * Math.PI) * (2 * m));

		MaxQueue que = new MaxQueue(4 * m);
		for (int i = 0; i < len; i++) {
			que.push(arr[i]);
		}

		double[] arrMax = new double[2 * m + 1];

		for (int i = 0; i < 2 * m; i++) {
			if (i != 0) {
				que.push(arr[(i + len - 1) % (2 * m)]);
				que.pop();
			}
			arrMax[i] = que.getMax();
		}

		arrMax[2 * m] = arrMax[0];

		double ret = 0;
		for (int i = 0; i < m; i++) {
			ret += arrMax[2 * i] + 4 * arrMax[2 * i + 1] + arrMax[2 * i + 2];
		}

		ret *= 2 * Math.PI / (6 * m);
		return ret;
	}

	double getDistSqr(double x1, double y1, double x2, double y2) {
		double cross = x1 * y2 - x2 * y1;
		double len = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
		return cross * cross / len;
	}

	double solve(int[] xs, int[] ys, double angle) {
		int n = xs.length;
		xs = Arrays.copyOf(xs, n + 1);
		ys = Arrays.copyOf(ys, n + 1);

		xs[n] = xs[0];
		ys[n] = ys[0];

		Point[] pts = new Point[2 * n];

		int ptr = 0;

		for (int i = 0; i < n; i++) {
			long px = xs[i];
			long py = ys[i];
			long vx = xs[i + 1] - xs[i];
			long vy = ys[i + 1] - ys[i];

			long num = -(px * vx + py * vy);
			long denom = vx * vx + vy * vy;

			pts[ptr++] = new Point(px, py);

			if (0 < num && num < denom) {
				double t = 1.0 * num / denom;
				pts[ptr++] = new Point(px + vx * t, py + vy * t);
			}
		}

		return solve2(Arrays.copyOf(pts, ptr), angle);
	}

	double solve2(Point[] pts, double sweep) {
		// System.err.println(Arrays.toString(pts));

		// for (Point p : pts) {
		// System.err.println(p);
		// }
		// System.err.println();

		int n = pts.length;
		Point[] p = new Point[3 * n];
		for (int i = 0; i < 3 * n; i++) {
			p[i] = new Point(pts[i % n].x, pts[i % n].y);
			while (i > 0 && p[i].dir <= p[i - 1].dir) {
				p[i].dir += DBL_PI;
			}
		}

		MaxQueue que = new MaxQueue(3 * n);

		double cDir = p[0].dir;
		int pHead = 0;
		int pTail = 0;

		while (cDir + sweep > p[pTail].dir) {
			if (pTail != 0) {
				que.push(p[pTail].abs);
			}
			pTail++;
		}

		pTail--;

		double ret = 0;

		while (pHead < n) {
			if (pHead == pTail) {
				if (p[pHead].abs < p[pHead + 1].abs) {
					ret += 0.5 * Math.abs(segmDir(p[pHead], p[pHead + 1],
							cDir + sweep).cross(p[pHead + 1]));
				} else {
					ret += 0.5 * Math.abs(segmDir(p[pHead], p[pHead + 1], cDir)
							.cross(segmDir(p[pHead], p[pHead + 1],
									p[pHead + 1].dir - sweep)));
				}
				que.push(p[pTail + 1].abs);
				cDir = p[pTail + 1].dir - sweep;
				pTail++;
				continue;
			}

			double distHead = p[pHead + 1].dir - cDir;
			double distTail = p[pTail + 1].dir - (cDir + sweep);
			if (distHead < distTail) {
				ret += go(
						segmDir(p[pHead], p[pHead + 1], cDir),
						p[pHead + 1],
						segmDir(p[pTail], p[pTail + 1], cDir + sweep),
						segmDir(p[pTail], p[pTail + 1], p[pHead + 1].dir
								+ sweep), que.getMax(), distHead);

				que.pop();
				cDir = p[pHead + 1].dir;
				pHead++;
			} else {
				ret += go(
						segmDir(p[pHead], p[pHead + 1], cDir),
						segmDir(p[pHead], p[pHead + 1], p[pTail + 1].dir
								- sweep),
						segmDir(p[pTail], p[pTail + 1], cDir + sweep),
						p[pTail + 1], que.getMax(), distTail);
				que.push(p[pTail + 1].abs);
				cDir = p[pTail + 1].dir - sweep;
				pTail++;
			}
		}
		return ret;
	}
	
	
	static final double EPS = 1e-10;
	double safeSqrt(double x) {
		if (x < -EPS) {
			throw new AssertionError();
		}
		return Math.sqrt(Math.max(x, 0));
	}

	double go(Point low, Point high, double mid) {
		if (low.abs + EPS >= mid) {
			return 0.5 * Math.abs(low.cross(high));
		}
		if (high.abs - EPS <= mid) {
			return 0.5 * mid * mid * getAngle(low, high);
		}

		double m;
		{
			double vx = high.x - low.x;
			double vy = high.y - low.y;

			double A = vx * vx + vy * vy;
			double B = vx * low.x + vy * low.y;
			double C = low.dot(low) - mid * mid;

			m = (-B + safeSqrt(B * B - A * C)) / A;
		}

		Point midP = new Point(low.x + (high.x - low.x) * m, low.y
				+ (high.y - low.y) * m);
		return 0.5 * (mid * mid * getAngle(midP, low) + Math.abs(midP
				.cross(high)));
	}

	double getAngle(Point p, Point q) {
		return Math.abs(Math.atan2(p.cross(q), p.dot(q)));
	}

	double go(Point a, Point b, Point c, Point d, double mid, double angle) {
		boolean useAB = a.abs > b.abs;
		boolean useCD = c.abs < d.abs;

		if (!useAB && !useCD) {
			return 0.5 * angle * mid * mid;
		}

		if (useAB && !useCD) {
			return go(b, a, mid);
		}
		if (!useAB && useCD) {
			return go(c, d, mid);
		}

		if (b.abs > d.abs) {
			return go(b, a, mid);
		}

		if (c.abs > a.abs) {
			return go(c, d, mid);
		}

		c = a.makeLen(c.abs);
		d = b.makeLen(d.abs);

		Point x = linesCross(a, b, c, d);

		return go(x, a, mid) + go(x, d, mid);
	}

	Point linesCross(Point a, Point b, Point c, Point d) {
		double s1 = a.to(c).cross(a.to(d));
		double s2 = b.to(d).cross(b.to(c));
		double t = s1 / (s1 + s2);
		return new Point(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
	}

	Point segmDir(Point p, Point q, double dir) {
		double A = p.y - q.y;
		double B = q.x - p.x;
		double C = p.x * A + p.y * B;

		double cos = Math.cos(dir);
		double sin = Math.sin(dir);

		double t = C / (A * cos + B * sin);
		return new Point(t * cos, t * sin);
	}

	void submit() {
		int t = nextInt();
		for (int ti = 1; ti <= t; ti++) {
			int n = nextInt();
			double angle = nextDouble();
			int[] xs = new int[n];
			int[] ys = new int[n];

			for (int i = 0; i < n; i++) {
				xs[i] = nextInt();
				ys[i] = nextInt();
			}
			
			out.println(solve(xs, ys, angle));
		}
//		 out.println(numSimpson(xs, ys, angle, 1_000_000));
	}

	static class MaxQueue {
		double[] que;
		int[] deq;

		int qh, qt;
		int dh, dt;

		public MaxQueue(int n) {
			que = new double[n];
			deq = new int[n];
			qh = qt = dh = dt = 0;
		}

		void push(double x) {
			que[qt++] = x;
			while (dh < dt && que[deq[dt - 1]] <= x) {
				dt--;
			}
			deq[dt++] = qt - 1;
		}

		void pop() {
			if (qh - qt == 0) {
				throw new AssertionError();
			}
			int idx = qh++;
			if (deq[dh] == idx) {
				dh++;
			}
		}

		double getMax() {
			if (qh == qt) {
				return Double.NEGATIVE_INFINITY;
			}
			return que[deq[dh]];
		}
	}

	void preCalc() {

	}

	// void stress() {
	//
	// double maxAbsErr = 0;
	//
	// int tst = 0;
	// while (true) {
	// int[][] data = new Gen().genPoly(3);
	// int[] xs = data[0];
	// int[] ys = data[1];
	// double sweep = rng.nextDouble() * 2 * Math.PI;
	//
	// double fast = solve(xs, ys, sweep);
	// double numeric = numSimpson(xs, ys, sweep, 1_000_000);
	//
	// // Apfloat slow = new HighPrecision().solve(xs, ys, sweep);
	//
	// double relErr = Math.abs(fast - numeric) / fast;
	// if (relErr > maxAbsErr) {
	// maxAbsErr = relErr;
	// System.err.println(tst + ": " + maxAbsErr);
	// System.err.println(fast);
	// System.err.println(numeric);
	// // System.err.println(slow);
	// }
	//
	// tst++;
	// }
	// }

	void test() {
		Point[] a = new Point[8];
		a[0] = new Point(-1, -1);
		a[1] = new Point(0, -1);
		a[2] = new Point(Math.sqrt(3), -1);
		a[3] = new Point(Math.sqrt(3), 0);
		a[4] = new Point(Math.sqrt(3), Math.sqrt(3));
		a[5] = new Point(0, Math.sqrt(3));
		a[6] = new Point(-1, Math.sqrt(3));
		a[7] = new Point(-1, 0);
		out.println(solve2(a, Math.PI / 2));
		out.println(10 * Math.PI / 3 + 2);
	}

	Main() throws IOException {
//		 br = new BufferedReader(new FileReader("in.txt"));
//		 out = new PrintWriter("out.txt");
		br = new BufferedReader(new InputStreamReader(System.in));
		out = new PrintWriter(System.out);
		submit();
		// stress();
		// test();
		out.close();
	}

	static final Random rng = new Random();

	static int rand(int l, int r) {
		return l + rng.nextInt(r - l + 1);
	}

	public static void main(String[] args) throws IOException {
		new Main();
	}

	BufferedReader br;
	PrintWriter out;
	StringTokenizer st;

	String nextToken() {
		while (st == null || !st.hasMoreTokens()) {
			try {
				st = new StringTokenizer(br.readLine());
			} catch (IOException e) {
				throw new RuntimeException(e);
			}
		}
		return st.nextToken();
	}

	String nextString() {
		try {
			return br.readLine();
		} catch (IOException e) {
			throw new RuntimeException(e);
		}
	}

	int nextInt() {
		return Integer.parseInt(nextToken());
	}

	long nextLong() {
		return Long.parseLong(nextToken());
	}

	double nextDouble() {
		return Double.parseDouble(nextToken());
	}
}
