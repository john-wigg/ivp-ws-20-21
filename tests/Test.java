class Test {
	final int var1 = 0;
	final int var2 = 2;
	final int var3 = 127;
	final int var4 = 3;
	final int var5 = 100;
	
	int dyn1;
	int dyn2;
	int dyn3;
	
	public void meth1() {
		int m1;
		int m2;
		int m3;
		
		m1 = 10;
		m2 = 20;
		m3 = 0;
		/*
		while(m1 < m2) {
			m1 = m1 + 1;*/
			if (m1 == 15) {
				m3 = m3 + 1; 
			} else {
				m3 = m3 - 1;
			}
			
			if (m3 >= 5) {
				return;
			} else {
				m2 = m2 - 1;
			}
		/*}*/
		
		m3 = meth2(m1,m2);
		
		m2 = meth2(meth2(m1,m3), m2);
		
		return;
	}
	
	public int meth2(int m, int n) {
		return m * n + (m/n);
	}
}