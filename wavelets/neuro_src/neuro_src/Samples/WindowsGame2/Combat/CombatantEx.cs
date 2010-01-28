using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using RolePlayingGameData;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework;

namespace RolePlaying
{





    public abstract  class CombatantEx : Combatant
    {


        static public List<CombatantEx> _CombatantList = new List<CombatantEx>();


        float _AniTime = 0;
        protected int _LastDir = 0;
        /// <summary>
        /// The current state of this combatant.
        /// </summary>
        public override Character.CharacterState State
        {
            get { return Character.State; }
            set
            {
                //if (value == player.State)
                //{
                //    return;
                //}
                Character.State = value;

                _AniTime = 0;

                switch (Character.State)
                {

                    case RolePlayingGameData.Character.CharacterState.Attack:
                        //CombatSprite.PlayAnimation("Attack");
                        break;

                    case RolePlayingGameData.Character.CharacterState.Walking:
                        //CombatSprite.PlayAnimation("Walk");
                        break;


                    case RolePlayingGameData.Character.CharacterState.Idle:
                        //CombatSprite.PlayAnimation("Idle");
                        break;

                    case RolePlayingGameData.Character.CharacterState.Hit:
                        //CombatSprite.PlayAnimation("Hit");
                        break;

                    case RolePlayingGameData.Character.CharacterState.Dying:
                        //Character.StatisticsModifiers.HealthPoints =
                        //    -1 * Character.CharacterStatistics.HealthPoints;
                        //CombatSprite.PlayAnimation("Die");
                        break;
                }

                string aniName = value.ToString();
                if (_LastDir == 1)
                {
                    aniName += "2";
                }

                //System.Diagnostics.Debug.WriteLine(aniName);


                CombatSprite.PlayAnimation(aniName);


            }
        }




        public override void Update(GameTime gameTime)
        {
            base.Update(gameTime);

            if (0 <= _AniTime)
            {
                _AniTime += (float)gameTime.ElapsedGameTime.TotalSeconds;
            }

            switch (State)
            {

                   case RolePlayingGameData.Character.CharacterState.Attack:
                       {
                    if (0.5 < _AniTime)
                    {
                        _AniTime = -1;

                        Rectangle r = new Rectangle((int)this.Position.X, (int)this.Position.Y, 100, 100);

                        r.Offset(-50, -50);

                        //r.Offset((int)this.Position.X,(int) this.Position.Y);

                        r.Offset(_LastDir * 50, 0);


                        foreach(var obj in RolePlaying.CombatantEx._CombatantList)
                        {
                            if (obj != this)
                            {
                                if (r.Intersects(new Rectangle((int)obj.Position.X, (int)obj.Position.Y, 1, 1)))
                                {
                                    obj.State = RolePlayingGameData.Character.CharacterState.Hit;
                                }
                            }
                        }

                        


                        }
                    }

                    break;


            }

        }
    }
}
