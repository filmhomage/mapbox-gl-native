#include <mbgl/annotation/render_annotation_source.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/renderer/render_tile.hpp>

#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

namespace mbgl {

using namespace style;

RenderAnnotationSource::RenderAnnotationSource(Immutable<AnnotationSource::Impl> impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

const AnnotationSource::Impl& RenderAnnotationSource::impl() const {
    return static_cast<const AnnotationSource::Impl&>(*baseImpl);
}

bool RenderAnnotationSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderAnnotationSource::update(Immutable<style::Source::Impl> baseImpl_,
                                    const std::vector<Immutable<Layer::Impl>>& layers,
                                    const bool needsRendering,
                                    const bool needsRelayout,
                                    const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::Annotations,
                       util::tileSize,
                       { 0, 22 },
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<AnnotationTile>(tileID, parameters);
                       });
}

void RenderAnnotationSource::startRender(algorithm::ClipIDGenerator& generator, const mat4& projMatrix, const mat4& clipMatrix, const TransformState& transform) {
    generator.update(tilePyramid.getRenderTiles());
    tilePyramid.startRender(projMatrix, clipMatrix, transform);
}

void RenderAnnotationSource::finishRender(Painter& painter) {
    tilePyramid.finishRender(painter);
}

std::map<UnwrappedTileID, RenderTile>& RenderAnnotationSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderAnnotationSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                              const TransformState& transformState,
                                              const RenderStyle& style,
                                              const RenderedQueryOptions& options) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, style, options);
}

std::vector<Feature> RenderAnnotationSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}

void RenderAnnotationSource::setCacheSize(size_t size) {
    tilePyramid.setCacheSize(size);
}

void RenderAnnotationSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderAnnotationSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
