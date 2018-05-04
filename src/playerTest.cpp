/** test class for Player

*/

#include <stdio.h>
#include <assert>

public class PlayerTest{
	/**Test for newly created Player object

	*/


	public void test_noArgConstructor() {
		Player p = new Player();
		assert("",p.setName());
	
	}

	/**Test for setMoney and getMoney (positive amount)

	*/

	public void test_setMoney1() {
		Player p = new Player(;
		p.m_balance(500);
		assert(p.m_balance = 5500);

	}

	/**Test for balance money (negative amount)

	*/

	public void test_setMoney2(){
		Player p = new Player();
		p.m_balance(-750);
		assert(p.m_balance = 4250);

	}

}
