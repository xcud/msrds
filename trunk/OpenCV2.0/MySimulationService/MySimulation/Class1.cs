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

using dshow;
using dshow.Core;

namespace MySimulation
{
    public class Game1 : Microsoft.Xna.Framework.Game
    {
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;


        public delegate void OnDetected(surfCLR.SURF surf);
        
        public event OnDetected _EventOnDetected;


        public Game1()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
        }


        Texture2D _objectTex;// = new Texture2D(
        Texture2D _imageTex;// = new Texture2D(
        SpriteFont _Font1;


        CaptureDevice _cam;
             
        //Microsoft.Xna.Framework.Graphics.
        
        //DIRECT3DSURFACE9 Surface


        surfCLR.SURF _surf = new surfCLR.SURF();


        int _UpdateFrame = 0;
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




            _Font1 = Content.Load<SpriteFont>("SpriteFont1");


            FilterCollection filters = new FilterCollection(FilterCategory.VideoInputDevice);

            if( filters.Count != 0)
            {
                _cam = new CaptureDevice(GraphicsDevice);
                _cam.VideoSource = filters[0].MonikerString;
                _cam.Start();

                _cam.NewFrame += new CameraEventHandler(_cam_NewFrame);


            }

            


            _objectTex = _surf.InitToTexture(GraphicsDevice, "box2.png");


            // TODO: use this.Content to load your game content here
        }

        void _cam_NewFrame(GrabberInfo e)
        {


            Texture2D t = _surf.UpdateFrameToTexture(e._Ptr, e._Len,e._W,e._H); 
            GraphicsDevice.Textures[0] = null;
            _imageTex = t;

            if (_EventOnDetected != null)
            {
                _EventOnDetected(_surf);
            }

        }

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// all content.
        /// </summary>
        protected override void UnloadContent()
        {
            // TODO: Unload any non ContentManager content here

            _cam.SignalToStop();
            _cam.WaitForStop();
            _cam.Stop();

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

            // TODO: Add your update logic here
            //GraphicsDevice.Textures[0] = null;
            ////_imageTex = null;
            //_imageTex = _surf.UpdateFrameToTexture();

            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.CornflowerBlue);

            spriteBatch.Begin();
            // TODO: Add your drawing code here
            if (_imageTex != null)
            {
                spriteBatch.Draw(_objectTex, new Rectangle(0, 0,_objectTex.Width,_objectTex.Height), Color.White);
                spriteBatch.Draw(_imageTex, new Rectangle(0, _objectTex.Height, _imageTex.Width, _imageTex.Height), Color.White);

                spriteBatch.DrawString(_Font1, string.Format("X({0}),Y({1}),S({2})", _surf.GetCX(), _surf.GetCY(), _surf.GetCS()), new Vector2(10, 10), Color.Blue);
            }


            spriteBatch.DrawString(_Font1,string.Format("{0}",_UpdateFrame++),new Vector2(10,20), Color.Blue);

            spriteBatch.End();
            

            base.Draw(gameTime);
        }
    }
}
