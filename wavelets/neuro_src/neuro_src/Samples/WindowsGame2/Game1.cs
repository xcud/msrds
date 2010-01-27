using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;
using Microsoft.Xna.Framework.Net;
using Microsoft.Xna.Framework.Storage;
using System.IO;


namespace WindowsGame2
{
    /// <summary>
    /// This is the main type for your game
    /// </summary>
    public class Game1 : Microsoft.Xna.Framework.Game
    {
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;


        RolePlayingGameData.Player _Player = new RolePlayingGameData.Player();
        RolePlaying.CombatantPlayer _CPlayer;

        //List<RolePlaying.CombatantPlayer> _PlayerList = new List<RolePlaying.CombatantPlayer>();

        

        public Game1()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";


        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {
            // TODO: Add your initialization logic here

            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = new SpriteBatch(GraphicsDevice);



            _Player = Content.Load<RolePlayingGameData.Player>(@"Characters\Players\Kolatt").Clone() as RolePlayingGameData.Player;
            _CPlayer = new RolePlaying.CombatantPlayer(_Player);


            _CPlayer.Position = new Vector2(100, 100);

            RolePlaying.CombatantEx._CombatantList.Add(_CPlayer);

            

            RolePlaying.CombatantMonster _Mob = new RolePlaying.CombatantMonster(Content.Load<RolePlayingGameData.Player>(@"Characters\Players\Kolatt").Clone() as RolePlayingGameData.Player);


            _Mob.Position = new Vector2(100,100);

            RolePlaying.CombatantEx._CombatantList.Add(_Mob);

            
            // TODO: use this.Content to load your game content here
        }

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// all content.
        /// </summary>
        protected override void UnloadContent()
        {
            // TODO: Unload any non ContentManager content here
        }

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            // Allows the game to exit
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
                this.Exit();


            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed)
                this.Exit();


            foreach (var obj in RolePlaying.CombatantEx._CombatantList)
            {

                obj.Update( gameTime);
            }


            
            // TODO: Add your update logic here

            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {

            float elapsedSeconds = (float)gameTime.ElapsedGameTime.TotalSeconds;


            GraphicsDevice.Clear(Color.CornflowerBlue);

            spriteBatch.Begin();

            foreach (var obj in RolePlaying.CombatantEx._CombatantList)
            {

                obj.Draw(spriteBatch, gameTime);
            }

            
            //_Player.CombatSprite.UpdateAnimation(elapsedSeconds);

            //if (_Player.CombatSprite.IsPlaybackComplete)
            //{
            //    if (_Player.State == RolePlayingGameData.Character.CharacterState.Hit)
            //    {
            //        _Player.CombatSprite.PlayAnimation("Idle");
            //    }
            //    else if (_Player.State == RolePlayingGameData.Character.CharacterState.Dying)
            //    {
            //        _Player.CombatSprite.PlayAnimation("Idle");
            //    }
            //}


            //_Player.CombatSprite.Draw(spriteBatch, new Vector2(100, 100), 0);


            
            //_Player.ResetAnimation(true);
            //switch (_Player.State)
            //{
            //    case RolePlayingGameData.Character.CharacterState.Idle:
            //        _Player.MapSprite.UpdateAnimation(elapsedSeconds);
            //        _Player.MapSprite.Draw(spriteBatch, new Vector2(100, 100), 0);
            //        break;
            //    case RolePlayingGameData.Character.CharacterState.Walking:
            //        _Player.WalkingSprite.UpdateAnimation(elapsedSeconds);
            //        _Player.WalkingSprite.Draw(spriteBatch, new Vector2(100, 100), 0);

            //        break;

            //    case RolePlayingGameData.Character.CharacterState.Hit:
            //        //_Player.CombatSprite.PlayAnimation("Hit");
            //        _Player.CombatSprite.UpdateAnimation(elapsedSeconds);
            //        _Player.CombatSprite.Draw(spriteBatch, new Vector2(100, 100), 0);

            //        break;
            //}
            

            spriteBatch.End();

            // TODO: Add your drawing code here

            base.Draw(gameTime);
        }
    }
}
