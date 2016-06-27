inline SkyDome::TexMatCallback::TexMatCallback(osg::TexMat* tm)
:_texMat(tm)
{
}
inline SkyDome::SkyDome()
:started(false)
{
}
inline SkyDome::SkyDome(std::string const& filename)
:skydome(filename), started(false)
{
		buildSkyDome();
}
inline void SkyDome::setSkyDomeFile(std::string const& filename)
{
	skydome = filename;
}
inline osg::Node* SkyDome::getNode()
{
	return clearNode.get();
}